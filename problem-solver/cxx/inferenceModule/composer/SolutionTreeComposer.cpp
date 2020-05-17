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
#include "SolutionTreeComposer.hpp"

using namespace inference;
using namespace utils;
using namespace scAgentsCommon;

SolutionTreeComposer::SolutionTreeComposer(ScMemoryContext * ms_context) : ms_context(ms_context)
{
  lastNode = ScAddr();
}

bool SolutionTreeComposer::addNode(ScAddr const & rule, ScTemplateParams const & templateParams)
{
  ScAddr newNode = createSolutionNode(rule, templateParams);
  bool result = newNode.IsValid();
  if (result)
  {
    result = GenerationUtils::generateRelationBetween(
          ms_context,
          lastNode,
          newNode,
          CoreKeynodes::nrel_basic_sequence);
    lastNode = newNode;
  }
  return result;
}

ScAddr SolutionTreeComposer::createSolution(const bool targetAchieved)
{
  ScAddr solution = ms_context->CreateNode(ScType::NodeConst);
  if (lastNode.IsValid())
  {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, lastNode, solution);
  }
  ScType arcType = targetAchieved ? ScType::EdgeAccessConstPosPerm : ScType::EdgeAccessConstNegPerm;
  ms_context->CreateEdge(arcType, InferenceKeynodes::concept_success_solution, solution);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::concept_solution, solution);
  return solution;
}

ScAddr SolutionTreeComposer::createSolutionNode(ScAddr const & rule, ScTemplateParams const & templateParams)
{
  ScAddr solutionNode = ms_context->CreateNode(ScType::NodeConst);
  //TODO: Add params to solution node
  GenerationUtils::generateRelationBetween(ms_context, solutionNode, rule, CoreKeynodes::rrel_1);
  return solutionNode;
}