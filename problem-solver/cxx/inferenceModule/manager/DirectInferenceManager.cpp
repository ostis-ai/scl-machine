/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "DirectInferenceManager.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/LogicRuleUtils.hpp>

#include "utils/ContainersUtils.hpp"

using namespace inference;
using namespace utils;

DirectInferenceManager::DirectInferenceManager(ScMemoryContext * ms_context) : ms_context(ms_context)
{
  this->solutionTreeManager = new SolutionTreeGenerator(ms_context);
  this->templateManager = new TemplateManager(ms_context);
  this->templateSearcher = new TemplateSearcher(ms_context);
}

DirectInferenceManager::~DirectInferenceManager()
{
  delete this->solutionTreeManager;
  delete this->templateManager;
  delete this->templateSearcher;
}

ScAddr DirectInferenceManager::applyInference(
      const ScAddr & targetStatement,
      const ScAddr & ruleSet,
      const ScAddr & argumentSet)
{
  queue<ScAddr> uncheckedRules = createQueue(ruleSet);
  vector<ScAddr> argumentList = IteratorUtils::getAllWithType(ms_context, argumentSet, ScType::Node);
  vector<ScAddr> checkedRuleList;

  bool targetAchieved = isTargetAchieved(targetStatement, argumentList);
  ScAddr rule;
  bool isUsed;
  if (!targetAchieved)
  {
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
  }
  else
  { SC_LOG_DEBUG("Target is already achieved") }

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
  vector<ScTemplateParams> ifStatementParamsList = templateManager->createTemplateParamsList(ifStatement, argumentList);
  SC_LOG_DEBUG("Created " + to_string(ifStatementParamsList.size()) + " statement params variants");
  for (const auto& ifStatementParams : ifStatementParamsList)
  {
    vector<ScTemplateSearchResultItem> searchResult =
        templateSearcher->searchTemplate(ifStatement, ifStatementParams);
    if (!searchResult.empty())
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
      if (generateStatement(elseStatement, elseStatementParams))
      {
        this->solutionTreeManager->addNode(rule, ifStatementParams);
        isUsed = true;
        SC_LOG_DEBUG("Rule used");
      }
    }
  }
  return isUsed;
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
  ScTemplateParams templateParams = templateManager->createTemplateParams(targetStatement, argumentList);
  vector<ScTemplateSearchResultItem> searchResult =
      templateSearcher->searchTemplate(targetStatement, templateParams);
  if (!searchResult.empty())
  { result = true; }
  return result;
}
