/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <iostream>

#include <sc-memory/cpp/sc_addr.hpp>
#include <sc-memory/cpp/sc_stream.hpp>
#include <sc-kpm/sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-kpm/sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-kpm/sc-agents-common/keynodes/CoreKeynodes.hpp>
#include <sc-kpm/sc-agents-common/utils/LogicRuleUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"
#include "DirectInferenceManager.hpp"

using namespace inference;
using namespace utils;
using namespace scAgentsCommon;

DirectInferenceManager::DirectInferenceManager(ScMemoryContext * ms_context) : ms_context(ms_context)
{
  this->solutionTreeManager = new SolutionTreeComposer(ms_context);
}

DirectInferenceManager::~DirectInferenceManager()
{
  delete this->solutionTreeManager;
}


ScAddr DirectInferenceManager::applyInference(
      const ScAddr & targetStatement,
      const ScAddr & ruleSet,
      const ScAddr & argumentSet)
{
  queue<ScAddr> uncheckedRules = createQueue(ruleSet);
  vector<ScAddr> argumentList = IteratorUtils::getAllWithType(ms_context, argumentSet, ScType::Node);
  vector<ScAddr> checkedRuleList;

  bool targetAchieved = false;
  ScAddr rule;
  bool isUsed;
  while (!uncheckedRules.empty())
  {
    rule = uncheckedRules.front();
    isUsed = useRule(rule, argumentList);
    if (isUsed)
    {
      targetAchieved = isTargetAchieved(targetStatement, argumentList);
      if (targetAchieved)
      {
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
      checkedRuleList.push_back(rule);
    }
    uncheckedRules.pop();
  }

  return this->solutionTreeManager->createSolution(targetAchieved);
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

bool DirectInferenceManager::useRule(ScAddr const & rule, vector<ScAddr> const & argumentList)
{
  bool isUsed = false;
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
              ScAddr node;
              ifStatementParams.Get(varName, node);
              if (!node.IsValid())
              {
                node = firstResult[varName];
              }
              elseStatementParams.Add(varName, node);
            }

          }
        }

        isUsed = generateStatement(elseStatement, elseStatementParams);
        if (isUsed)
        {
          this->solutionTreeManager->addNode(rule, ifStatementParams);
        }
      }
    }
  }
  return isUsed;
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