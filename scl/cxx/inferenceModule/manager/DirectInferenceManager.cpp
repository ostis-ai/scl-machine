/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "DirectInferenceManager.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <logic/LogicExpression.hpp>
#include <utils/LogicRuleUtils.hpp>

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
  // returns all <ScType::Node>s from argumentSet
  vector<ScAddr> argumentList = IteratorUtils::getAllWithType(ms_context, argumentSet, ScType::Node);

  //
  bool targetAchieved = isTargetAchieved(targetStatement, argumentList);

  vector<queue<ScAddr>> rulesQueuesByPriority;
  try
  {
    rulesQueuesByPriority = createRulesQueuesListByPriority(ruleSet);
  }
  catch (std::runtime_error & ex)
  {
    SC_LOG_ERROR(ex.what());
    return this->solutionTreeManager->createSolution(targetAchieved);
  }

  if (rulesQueuesByPriority.empty())
  {
    SC_LOG_DEBUG("No rule sets found.");
    return this->solutionTreeManager->createSolution(targetAchieved);
  }

  vector<ScAddr> checkedRuleList;
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
            SC_LOG_DEBUG("Target achieved");
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
  { SC_LOG_DEBUG("Target is already achieved"); }

  return this->solutionTreeManager->createSolution(targetAchieved);
}


ScAddr DirectInferenceManager::applyInference(
      const ScAddr & ruleSet,
      const ScAddr & inputStructure,
      const ScAddr & outputStructure,
      const ScAddr & targetStatement)
{
  this->inputStructure = inputStructure;
  this->outputStructure = outputStructure;
  this->targetStatement = targetStatement;

  vector<ScAddr> argumentList;
  if (inputStructure.IsValid())
  {
    ScIterator3Ptr iterator3 = ms_context->Iterator3(inputStructure, ScType::EdgeAccessConstPosPerm, ScType::Node);
    while (iterator3->Next())
      templateSearcher->addParam(iterator3->Get(2));
    argumentList = IteratorUtils::getAllWithType(ms_context, inputStructure, ScType::Node);
    if (argumentList.empty())
      return this->solutionTreeManager->createSolution(false);
  }

  templateSearcher->setInputStructure(inputStructure);



  bool targetAchieved = isTargetAchieved(targetStatement, argumentList);

  if (!targetStatement.IsValid())
  {
    SC_LOG_DEBUG("Target is not valid");
    return this->solutionTreeManager->createSolution(targetAchieved);
  }

  if (targetAchieved)
    SC_LOG_DEBUG("Target is already achieved");
  else
  {
    if(!ruleSet.IsValid())
    {
      SC_LOG_DEBUG("rules set is not valid");
      return this->solutionTreeManager->createSolution(targetAchieved);
    }
    vector<queue<ScAddr>> rulesQueuesByPriority;
    try
    {
      rulesQueuesByPriority = createRulesQueuesListByPriority(ruleSet);
    }
    catch (std::runtime_error & ex)
    {
      SC_LOG_ERROR(ex.what());
      return this->solutionTreeManager->createSolution(targetAchieved);
    }

    if (rulesQueuesByPriority.empty())
    {
      SC_LOG_DEBUG("No rule sets found.");
      return this->solutionTreeManager->createSolution(targetAchieved);
    }

    vector<ScAddr> checkedRuleList;
    queue<ScAddr> uncheckedRules;

    ScAddr rule;
    ScAddr model = (inputStructure.IsValid() ? inputStructure : InferenceKeynodes::knowledge_base_IMS);
    bool isUsed;
    SC_LOG_DEBUG("Start rule applying. There is " + to_string(rulesQueuesByPriority.size()) + " rule(s)");
    for (size_t ruleQueueIndex = 0; ruleQueueIndex < rulesQueuesByPriority.size() && !targetAchieved; ruleQueueIndex++)
    {
      uncheckedRules = rulesQueuesByPriority[ruleQueueIndex];
      while (!uncheckedRules.empty())
      {
        rule = uncheckedRules.front();
        clearSatisfiabilityInformation(rule);
        SC_LOG_DEBUG("Using rule " + ms_context->HelperGetSystemIdtf(rule));
        isUsed = useRule(rule, argumentList);
        if (isUsed)
        {
          addSatisfiabilityInformation(rule, model, true);
          targetAchieved = isTargetAchieved(targetStatement, argumentList);
          if (targetAchieved)
          {
            SC_LOG_DEBUG("Target achieved in applyInterference([4])");
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
          addSatisfiabilityInformation(rule, model, false);
          checkedRuleList.push_back(rule);
        }

        uncheckedRules.pop();
      }
    }
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

bool DirectInferenceManager::useRule(ScAddr const & rule, vector<ScAddr> /*const*/ & argumentList)
{
  SC_LOG_DEBUG("Trying to use rule: " + ms_context->HelperGetSystemIdtf(rule));
  bool isUsed = false;
  ScAddr keyScElement = IteratorUtils::getAnyByOutRelation(ms_context, rule, InferenceKeynodes::rrel_main_key_sc_element);
  if (!keyScElement.IsValid())
    return false;

  LogicExpression logicExpression(
        ms_context,
        templateSearcher,
        templateManager,
        argumentList,
        outputStructure
  );

  auto root = logicExpression.build(keyScElement);
  auto result = root->compute();
  SC_LOG_DEBUG(std::string("Whole statement is ") + (result.value ? "right" : "wrong"));

  return result.value;
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
  auto vectorOfTemplateParams = templateManager->createTemplateParams(targetStatement, argumentList);
  for (auto const & templateParams : vectorOfTemplateParams)
  {
    auto searchResult = templateSearcher->searchTemplate(targetStatement, templateParams);
    if(!searchResult.empty())
      return true;
  }
  return false;
}

void DirectInferenceManager::clearSatisfiabilityInformation(ScAddr const & rule, ScAddr const & model)
{
  ScIterator5Ptr iterator5Ptr = ms_context->Iterator5(
        rule,
        ScType::EdgeDCommon,
        model,
        ScType::EdgeAccess,
        InferenceKeynodes::nrel_satisfiable_formula);

  while (iterator5Ptr->Next())
    ms_context->EraseElement(iterator5Ptr->Get(1));
}

void DirectInferenceManager::addSatisfiabilityInformation(ScAddr const & rule, ScAddr const & model, bool isSatisfiable)
{
  clearSatisfiabilityInformation(rule, model);
  ScAddr arc1 = ms_context->CreateEdge(ScType::EdgeDCommonConst, rule, model);
  ScType arcType = (isSatisfiable ? ScType::EdgeAccessConstPosTemp : ScType::EdgeAccessConstNegTemp);
  ms_context->CreateEdge(
        arcType,
        InferenceKeynodes::nrel_satisfiable_formula,
        arc1);
}
