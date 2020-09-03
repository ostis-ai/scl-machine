/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "DirectInferenceManager.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <sc-agents-common/utils/LogicRuleUtils.hpp>

#include "utils/ContainersUtils.hpp"

using namespace inference;
using namespace utils;
using namespace scAgentsCommon;

DirectInferenceManager::DirectInferenceManager(ScMemoryContext * ms_context) : ms_context(ms_context)
{
  this->solutionTreeManager = new SolutionTreeGenerator(ms_context);
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
        SC_LOG_DEBUG("Target achieved");
        break;
      }
      else
      {
        ContainersUtils::addToQueue(checkedRuleList, uncheckedRules);
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

  ContainersUtils::addToQueue(elementList, queue);
  return queue;
}

bool DirectInferenceManager::useRule(ScAddr const & rule, vector<ScAddr> const & argumentList)
{
  SC_LOG_DEBUG("Trying to use rule: " + ms_context->HelperGetSystemIdtf(rule));
  bool isUsed = false;
  ScAddr ifStatement = LogicRuleUtils::getIfStatement(ms_context, rule);
  std::vector<ScTemplateParams> ifStatementParamsList = createTemplateParamsList(ifStatement, argumentList);
  SC_LOG_DEBUG("Created " + std::to_string(ifStatementParamsList.size()) + " statement params variants");
  for (const auto& ifStatementParams : ifStatementParamsList)
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

        if(generateStatement(elseStatement, elseStatementParams))
        {
          this->solutionTreeManager->addNode(rule, ifStatementParams);
          isUsed = true;
          SC_LOG_DEBUG("Rule used");
        }
      }
    }
  }
  return isUsed;
}

std::vector<ScTemplateParams>
DirectInferenceManager::createTemplateParamsList(ScAddr const & scTemplate, const vector<ScAddr> & argumentList)
{
  std::vector<std::map<ScAddr, string, AddrComparator>> replacementsList;
  ScIterator3Ptr varIterator = ms_context->Iterator3(
        scTemplate,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    string varName = ms_context->HelperGetSystemIdtf(var);
    std::vector<ScAddr> argumentOfVarList;
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
        if ((ms_context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm)) && (argument.IsValid()))
        {
          argumentOfVarList.push_back(argument);
        }
      }
    }
    if(!argumentList.empty())
    {
      addVarToReplacementsList(replacementsList, varName, argumentOfVarList);
    }
  }

  return createTemplateParamsList(replacementsList);
}

void DirectInferenceManager::addVarToReplacementsList(
      std::vector<std::map<ScAddr, string, AddrComparator>> & replacementsList,
      string & varName,
      std::vector<ScAddr> & argumentOfVarList)
{
  if(!argumentOfVarList.empty())
  {
    std::vector<std::map<ScAddr, string, AddrComparator>> newReplacementsList;

    if (!replacementsList.empty())
    {
      for (auto & replacementsMap : replacementsList)
      {
        for (auto & argumentOfVar : argumentOfVarList)
        {
          if(!replacementsMap.count(argumentOfVar))
          {
            std::map<ScAddr, string, AddrComparator> newReplacementsMap = replacementsMap;
            newReplacementsMap.insert(std::make_pair(argumentOfVar, varName));
            newReplacementsList.push_back(newReplacementsMap);
          }
        }
      }
    }
    else
    {
      for (auto & argumentOfVar : argumentOfVarList)
      {
        std::map<ScAddr, string, AddrComparator> newReplacementsMap;
        newReplacementsMap.insert(std::make_pair(argumentOfVar, varName));
        newReplacementsList.push_back(newReplacementsMap);
      }
    }

    replacementsList = newReplacementsList;
  }
}

std::vector<ScTemplateParams>
DirectInferenceManager::createTemplateParamsList(std::vector<std::map<ScAddr, string, AddrComparator>> & replacementsList)
{
  SC_LOG_DEBUG("Creating template params fo rule usage")
  std::vector<ScTemplateParams> templateParamsList;
  for (auto & replacementsMap : replacementsList)
  {
    ScTemplateParams scTemplateParams;
    for (std::pair<ScAddr, string> replacement : replacementsMap)
    {
      SC_LOG_DEBUG(replacement.second + " is " + std::to_string(replacement.first.GetRealAddr().seg) + "/" + std::to_string(replacement.first.GetRealAddr().offset));
      scTemplateParams.Add(replacement.second, replacement.first);
    }
    templateParamsList.push_back(scTemplateParams);
    SC_LOG_DEBUG("***")
  }
  SC_LOG_DEBUG("Created template params fo rule usage")
  return templateParamsList;
}

ScTemplateParams
DirectInferenceManager::createTemplateParams(ScAddr const & scTemplate, const vector<ScAddr> & argumentList)
{
  std::map<ScAddr, string, AddrComparator> replacementsMap;
  SC_LOG_DEBUG("***\ncreating template params\ntemplate name: " + ms_context->HelperGetSystemIdtf(scTemplate));

  ScIterator3Ptr varIterator = ms_context->Iterator3(
        scTemplate,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    SC_LOG_DEBUG("found variable: " + ms_context->HelperGetSystemIdtf(var));
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
          if(!replacementsMap.count(argument))
          {
            argumentOfVar = argument;
            break;
          }
        }
      }
      if (argumentOfVar.IsValid())
      {
        string varName = ms_context->HelperGetSystemIdtf(var);
        replacementsMap.insert(std::make_pair(argumentOfVar, varName));
        SC_LOG_DEBUG("variable class: " + ms_context->HelperGetSystemIdtf(varClass));
        SC_LOG_DEBUG(varName + " is " + std::to_string(argumentOfVar.GetRealAddr().seg) + "/" + std::to_string(argumentOfVar.GetRealAddr().offset));
        break;
      }
    }
  }
  SC_LOG_DEBUG("***");

  ScTemplateParams templateParams;
  for (const auto& replacement : replacementsMap)
  {
    templateParams.Add(replacement.second, replacement.first);
  }
  return templateParams;
}



bool DirectInferenceManager::generateStatement(ScAddr const & statement, ScTemplateParams const & templateParams)
{
  bool result = false;
  ScTemplate searchTemplate;
  ms_context->HelperBuildTemplate(searchTemplate, statement, templateParams);
  ScTemplateSearchResult templateSearchResult;
  if (!ms_context->HelperSearchTemplate(searchTemplate, templateSearchResult))
  {
    ScTemplate statementTemplate;
    ms_context->HelperBuildTemplate(statementTemplate, statement);
    ScTemplateGenResult templateGenResult;
    result = ms_context->HelperGenTemplate(statementTemplate, templateGenResult, templateParams);
  }
  return result;
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