/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "SolutionTreeGenerator.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;
using namespace utils;
using namespace scAgentsCommon;

SolutionTreeGenerator::SolutionTreeGenerator(ScMemoryContext * ms_context)
      : ms_context(ms_context)
{
  solution = ms_context->CreateNode(ScType::NodeConst);
}

bool SolutionTreeGenerator::addNode(ScAddr const & formula, ScTemplateParams const & templateParams)
{
  ScAddr newSolutionNode = createSolutionNode(formula, templateParams);
  bool result = newSolutionNode.IsValid();
  if (result)
  {
    if (!lastSolutionNode.IsValid())
    {
      result = GenerationUtils::generateRelationBetween(ms_context, solution, newSolutionNode, CoreKeynodes::rrel_1);
    }
    else
    {
      ScIterator3Ptr lastSolutionNodeArcIterator = ms_context->Iterator3(
            solution,
            ScType::EdgeAccessConstPosPerm,
            lastSolutionNode);
      if (lastSolutionNodeArcIterator->Next())
      {
        ScAddr lastSolutionNodeArc = lastSolutionNodeArcIterator->Get(1);
        ScAddr newSolutionNodeArc = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, solution, newSolutionNode);
        GenerationUtils::generateRelationBetween(
              ms_context,
              lastSolutionNodeArc,
              newSolutionNodeArc,
              CoreKeynodes::nrel_basic_sequence);
      }
      else
      {
        result = false;
      }
    }

    lastSolutionNode = newSolutionNode;
  }
  return result;
}

ScAddr SolutionTreeGenerator::createSolutionNode(ScAddr const & formula, ScTemplateParams const & templateParams)
{
  ScAddr solutionNode = ms_context->CreateNode(ScType::NodeConst);
  // TODO: Add params to solution node
  GenerationUtils::generateRelationBetween(ms_context, solutionNode, formula, CoreKeynodes::rrel_1);
  return solutionNode;
}

ScAddr SolutionTreeGenerator::createSolution(bool const targetAchieved)
{
  ScType arcType = targetAchieved ? ScType::EdgeAccessConstPosPerm : ScType::EdgeAccessConstNegPerm;
  ms_context->CreateEdge(arcType, InferenceKeynodes::concept_success_solution, solution);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::concept_solution, solution);
  return solution;
}
