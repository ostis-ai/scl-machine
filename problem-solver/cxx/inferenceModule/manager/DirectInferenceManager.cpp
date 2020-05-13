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

#include "keynodes/InferenceKeynodes.hpp"
#include "DirectInferenceManager.hpp"

using namespace inference;
using namespace utils;
using namespace scAgentsCommon;

DirectInferenceManager::DirectInferenceManager(ScMemoryContext * ms_context) : ms_context(ms_context)
{}


ScAddr & DirectInferenceManager::applyInference(
      const ScAddr & targetTemplate,
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
    solutionNode = useRule(currentRule, argumentList);
    if (solutionNode.IsValid())
    {
      ScAddr gotoArc = ms_context->CreateEdge(ScType::EdgeDCommonConst, lastSolutionNode, solutionNode);
      ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_basic_sequence, gotoArc);
      lastSolutionNode = solutionNode;
      targetAchieved = isTargetAchieved(targetTemplate, argumentList);
      if (targetAchieved)
      {
        checkedRuleList.clear();
        checkedRuleList.shrink_to_fit();
        argumentList.clear();
        argumentList.shrink_to_fit();
        break;
      }
      else{
        addToQueue(checkedRuleList, uncheckedRules);
        checkedRuleList.clear();
      }
    }
    else
    {
      checkedRuleList.push_back(currentRule);
    }
  }

  ScAddr solution = ms_context->CreateNode(ScType::NodeConst);
  if (lastSolutionNode.IsValid())
  {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, solutionNode, solution);
  }
  ScType arcType = targetAchieved ? ScType::EdgeAccessConstPosPerm : ScType::EdgeAccessConstNegPerm;
  ms_context->CreateEdge(arcType, InferenceKeynodes::success_solution, solution);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::solution, solution);
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

ScAddr & DirectInferenceManager::useRule(ScAddr const & rule, vector<ScAddr> const & argumentList)
{
  ScAddr solutionNode = ScAddr();
  return solutionNode;
}

bool DirectInferenceManager::isTargetAchieved(ScAddr const & targetTemplate, vector<ScAddr> const & argumentList)
{
  return false;
}