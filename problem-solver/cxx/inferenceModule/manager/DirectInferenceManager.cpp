/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <iostream>

#include <sc-memory/cpp/sc_addr.hpp>
#include <sc-memory/cpp/sc_stream.hpp>
#include <sc-kpm/sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-kpm/sc-agents-common/keynodes/CoreKeynodes.hpp>
#include <sc-kpm/sc-agents-common/utils/LogicRuleUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"
#include "DirectInferenceManager.hpp"

using namespace inference;
using namespace utils;
using namespace scAgentsCommon;

DirectInferenceManager::DirectInferenceManager(ScMemoryContext * ms_context) : ms_context(ms_context)
{}


ScAddr DirectInferenceManager::applyInference(
      const ScAddr & targetStatement,
      const ScAddr & ruleSet,
      const ScAddr & argumentSet)
{

  queue<ScAddr> uncheckedRules = createQueue(ruleSet);
  vector<ScAddr> checkedRuleList;
  vector<ScAddr> argumentList = IteratorUtils::getAllWithType(ms_context, argumentSet, ScType::Node);

  ScAddr lastSolutionNode;
  ScAddr currentRule, solutionNode;
  bool targetAchieved = false;
  while (!uncheckedRules.empty())
  {
    currentRule = uncheckedRules.front();
    cout << ms_context->HelperGetSystemIdtf(currentRule) << endl;
    solutionNode = useRule(currentRule, argumentList);
    cout << solutionNode.IsValid() << endl;
    if (solutionNode.IsValid())
    {
      ScAddr gotoArc = ms_context->CreateEdge(ScType::EdgeDCommonConst, lastSolutionNode, solutionNode);
      ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_basic_sequence, gotoArc);
      lastSolutionNode = solutionNode;
      targetAchieved = isTargetAchieved(targetStatement, argumentList);
      if (targetAchieved)
      {
        checkedRuleList.clear();
        checkedRuleList.shrink_to_fit();
        argumentList.clear();
        argumentList.shrink_to_fit();
        break;
      }
      else
      {
        addToQueue(checkedRuleList, uncheckedRules);
        checkedRuleList.clear();
      }
    }
    else
    {
      checkedRuleList.push_back(currentRule);
    }
    uncheckedRules.pop();
  }

  ScAddr solution = ms_context->CreateNode(ScType::NodeConst);
  if (lastSolutionNode.IsValid())
  {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, solutionNode, solution);
  }
  ScType arcType = targetAchieved ? ScType::EdgeAccessConstPosPerm : ScType::EdgeAccessConstNegPerm;
  ms_context->CreateEdge(arcType, InferenceKeynodes::concept_success_solution, solution);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::concept_solution, solution);
  return solution;
}

queue<ScAddr> DirectInferenceManager::createQueue(ScAddr const & set)
{
  queue<ScAddr> queue;
  vector<ScAddr> elementList = IteratorUtils::getAllWithType(ms_context, set, ScType::Node);
  addToQueue(elementList, queue);
  elementList.clear();
  elementList.shrink_to_fit();
  return queue;
}

void DirectInferenceManager::addToQueue(vector<ScAddr> const & elementList, queue<ScAddr> & queue)
{
  for (auto element : elementList)
  {
    queue.push(element);
  }
}

ScAddr DirectInferenceManager::useRule(ScAddr const & rule, vector<ScAddr> const & argumentList)
{
  ScAddr solutionNode;
  ScAddr ifStatement = LogicRuleUtils::getIfStatement(ms_context, rule);
  ScTemplateParams ifStatementParams = createTemplateParams(ifStatement, argumentList);
  if (!ifStatementParams.IsEmpty())
  {
    ScTemplate ifStatementTemplate;
    if (ms_context->HelperBuildTemplate(ifStatementTemplate, ifStatement, ifStatementParams))
    {
      ScTemplateSearchResult searchResult;
      if (ms_context->HelperSearchTemplate(ifStatementTemplate, searchResult))
      {
        ScAddr elseStatement = LogicRuleUtils::getElseStatement(ms_context, rule);

        ScTemplateParams elseStatementParams;
        ScTemplateSearchResultItem firstResult = searchResult[0];
        if (firstResult.Size() > 0)
        {
          vector<ScAddr> varList = IteratorUtils::getAllWithType(ms_context, elseStatement, ScType::NodeVar);
          for (auto var : varList)
          {
            if (ms_context->HelperCheckEdge(ifStatement, var, ScType::EdgeAccessConstPosPerm))
            {
              string varName = ms_context->HelperGetSystemIdtf(var);
              cout << varName << endl;
              ScAddr node;
              ifStatementParams.Get(varName, node);
              if (!node.IsValid()){
                node = firstResult[varName];
              }
              elseStatementParams.Add(varName, node);
            }

          }
        }

        bool isGenerated = generateStatement(elseStatement, elseStatementParams);
        if (isGenerated)
        {
          solutionNode = createSolutionNode(rule, ifStatementParams);
        }
      }
    }
  }
  return solutionNode;
}

ScTemplateParams
DirectInferenceManager::createTemplateParams(ScAddr const & scTemplate, const vector<ScAddr> & argumentList)
{
  ScTemplateParams templateParams;
  ScIterator3Ptr varIterator = ms_context->Iterator3(
        scTemplate,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    ScAddr argumentOfVar;
    ScIterator5Ptr classesIterator = ms_context->Iterator5(
          ScType::NodeConstClass,
          ScType::EdgeAccessVarPosPerm,
          var,
          ScType::EdgeAccessConstPosPerm,
          scTemplate);
    while (classesIterator->Next())
    {
      ScAddr varClass = classesIterator->Get(0);
      for (auto & argument : argumentList)
      {
        if (ms_context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm))
        {
          argumentOfVar = argument;
          break;
        }
      }
      if (argumentOfVar.IsValid())
      {
        string varName = ms_context->HelperGetSystemIdtf(var);
        templateParams.Add(varName, argumentOfVar);
        break;
      }
    }
  }
  return templateParams;
}

bool DirectInferenceManager::generateStatement(ScAddr const & statement, ScTemplateParams const & templateParams)
{
  ScTemplate statementTemplate;
  ms_context->HelperBuildTemplate(statementTemplate, statement);
  ScTemplateGenResult result;
  return ms_context->HelperGenTemplate(statementTemplate, result, templateParams);
}

ScAddr DirectInferenceManager::createSolutionNode(ScAddr const & rule, ScTemplateParams const & templateParams)
{
  ScAddr solutionNode = ms_context->CreateNode(ScType::NodeConst);
  //TODO: Add params to solution node
  ScAddr arc = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, solutionNode, rule);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::rrel_1, arc);
  return solutionNode;
}

bool DirectInferenceManager::isTargetAchieved(ScAddr const & targetStatement, vector<ScAddr> const & argumentList)
{
  bool result = false;
  ScTemplateParams templateParams = createTemplateParams(targetStatement, argumentList);
  ScTemplate targetStatementTemplate;
  if (ms_context->HelperBuildTemplate(targetStatementTemplate, targetStatement, templateParams))
  {
    ScTemplateSearchResult searchResult;
    result = ms_context->HelperSearchTemplate(targetStatementTemplate, searchResult);
  }
  return result;
}

void DirectInferenceManager::printSetElements(ScAddr const & set)
{
  vector<ScAddr> elementList = IteratorUtils::getAllWithType(ms_context, set, ScType::Unknown);
  for (auto element : elementList)
  {
    cout << ms_context->HelperGetSystemIdtf(element) << endl;
  }
}