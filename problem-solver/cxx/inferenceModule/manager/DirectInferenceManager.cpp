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
#include <logic/LogicExpression.hpp>

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
  vector<ScAddr> argumentList = IteratorUtils::getAllWithType(ms_context, argumentSet, ScType::Node);
  vector<ScAddr> checkedRuleList;

  bool targetAchieved = isTargetAchieved(targetStatement, argumentList);

  vector<queue<ScAddr>> rulesQueuesByPriority;
  try
  {
    rulesQueuesByPriority = createRulesQueuesListByPriority(ruleSet);
  }
  catch (std::runtime_error & ex)
  {
    SC_LOG_ERROR(ex.what())
    return this->solutionTreeManager->createSolution(targetAchieved);
  }

  if (rulesQueuesByPriority.empty())
  {
    SC_LOG_DEBUG("No rule sets found.")
    return this->solutionTreeManager->createSolution(targetAchieved);
  }

  queue<ScAddr> uncheckedRules = rulesQueuesByPriority[0];

  ScAddr rule;
  bool isUsed;
  if (!targetAchieved)
  {
    for (size_t ruleQueueIndex = 0; ruleQueueIndex < rulesQueuesByPriority.size() && !targetAchieved; ruleQueueIndex++)
    {
      uncheckedRules = rulesQueuesByPriority[ruleQueueIndex];
      while (!uncheckedRules.empty())
      {
        rule = uncheckedRules.front();
        isUsed = useRule(rule, argumentList);
        if (isUsed)
        {
          targetAchieved = isTargetAchieved(targetStatement, argumentList);
          if (targetAchieved)
          {
            SC_LOG_DEBUG("Target achieved")
            break;
          }
          else
          {
            ContainersUtils::addToQueue(checkedRuleList, uncheckedRules);
            ruleQueueIndex = 0;
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
  SC_LOG_DEBUG("Trying to use rule: " + ms_context->HelperGetSystemIdtf(rule))
  bool isUsed = false;

  ScAddr ifStatement = LogicRuleUtils::getIfStatement(ms_context, rule);

  //collect all the templates within if condition
  //generate for them sets of ScTemplateParams
  //choose the set when IsEmpty() is false, memorize the template the set was generated for
  LogicExpression logicExpression(
        ms_context,
        templateSearcher,
        templateManager,
        argumentList
  );
  auto root = logicExpression.build(ifStatement);

  SC_LOG_DEBUG("Created " + to_string(logicExpression.GetParamsSet().size()) + " statement params variants")

  for (const auto & ifStatementParams : logicExpression.GetParamsSet())
  {
    auto checkResult = (*root).check(ifStatementParams);
    ScTemplateSearchResultItem resultItem = checkResult.templateSearchResult;
    SC_LOG_DEBUG(std::string("Whole statement is ") + (checkResult.value ? "right" : "wrong"))

    if (checkResult.value)
    {
      ScAddr elseStatement = LogicRuleUtils::getElseStatement(ms_context, rule);

      ScTemplateParams elseStatementParams;
      if (resultItem.Size() > 0)
      {
        vector<ScAddr> varList = IteratorUtils::getAllWithType(ms_context, elseStatement, ScType::NodeVar);
        for (auto const & var : varList)
        {
          if (ms_context->HelperCheckEdge(checkResult.formulaTemplate, var, ScType::EdgeAccessConstPosPerm))
          {
            std::string varName = ms_context->HelperGetSystemIdtf(var);
            ScAddr node;
            ifStatementParams.Get(varName, node);
            if (!node.IsValid())
            {
              node = resultItem[varName];
            }
            elseStatementParams.Add(varName, node);
          }
        }
      }
      if (generateStatement(elseStatement, elseStatementParams))
      {
        this->solutionTreeManager->addNode(rule, ifStatementParams);
        isUsed = true;
        SC_LOG_DEBUG("Rule used")
      }
    }
  }
  return isUsed;
}


vector<queue<ScAddr>> DirectInferenceManager::createRulesQueuesListByPriority(ScAddr const & rulesSet)
{
  vector<queue<ScAddr>> rulesQueuesList;

  ScAddr setOfRules = IteratorUtils::getFirstFromSet(ms_context, rulesSet, true);
  while (setOfRules.IsValid())
  {
    rulesQueuesList.push_back(createQueue(setOfRules));
    setOfRules = IteratorUtils::getNextFromSet(ms_context, rulesSet, setOfRules);
  }

  return rulesQueuesList;
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
