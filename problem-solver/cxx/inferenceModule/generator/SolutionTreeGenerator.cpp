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
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::concept_solution, solution);
}

bool SolutionTreeGenerator::addNode(
    ScAddr const & formula,
    ScTemplateParams const & templateParams,
    ScAddrHashSet const & variables)
{
  ScAddr newSolutionNode = createSolutionNode(formula, templateParams, variables);
  bool result = newSolutionNode.IsValid();
  if (result)
  {
    if (!lastSolutionNode.IsValid())
    {
      result = GenerationUtils::generateRelationBetween(ms_context, solution, newSolutionNode, CoreKeynodes::rrel_1);
    }
    else
    {
      ScIterator3Ptr lastSolutionNodeArcIterator =
          ms_context->Iterator3(solution, ScType::EdgeAccessConstPosPerm, lastSolutionNode);
      if (lastSolutionNodeArcIterator->Next())
      {
        ScAddr lastSolutionNodeArc = lastSolutionNodeArcIterator->Get(1);
        ScAddr newSolutionNodeArc = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, solution, newSolutionNode);
        GenerationUtils::generateRelationBetween(
            ms_context, lastSolutionNodeArc, newSolutionNodeArc, CoreKeynodes::nrel_basic_sequence);
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

ScAddr SolutionTreeGenerator::createSolutionNode(
    ScAddr const & formula,
    ScTemplateParams const & templateParams,
    ScAddrHashSet const & variables)
{
  ScAddr const & solutionNode = ms_context->CreateNode(ScType::NodeConst);
  GenerationUtils::generateRelationBetween(ms_context, solutionNode, formula, CoreKeynodes::rrel_1);
  ScAddr const & replacementsNode = ms_context->CreateNode(ScType::NodeConst);
  GenerationUtils::generateRelationBetween(ms_context, solutionNode, replacementsNode, CoreKeynodes::rrel_2);
  for (ScAddr const & variable : variables)
  {
    ScAddr replacement;
    templateParams.Get(variable, replacement);
    if (replacement.IsValid())
    {
      ScAddr const & pair = ms_context->CreateNode(ScType::NodeConst);
      ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, replacementsNode, pair);
      GenerationUtils::generateRelationBetween(ms_context, pair, replacement, CoreKeynodes::rrel_1);
      GenerationUtils::generateRelationBetween(ms_context, pair, variable, CoreKeynodes::rrel_2);
      ms_context->CreateEdge(ScType::EdgeAccessConstPosTemp, variable, replacement);
    }
    else
      SC_THROW_EXCEPTION(
          utils::ExceptionItemNotFound,
          "SolutionTreeGenerator: formula " << ms_context->HelperGetSystemIdtf(formula) << " has var "
                                            << ms_context->HelperGetSystemIdtf(variable)
                                            << " but scTemplateParams don't have replacement for this var");
  }

  return solutionNode;
}

ScAddr SolutionTreeGenerator::createSolution(ScAddr const & outputStructure, bool const targetAchieved)
{
  ScType arcType = targetAchieved ? ScType::EdgeAccessConstPosPerm : ScType::EdgeAccessConstNegPerm;
  ms_context->CreateEdge(arcType, InferenceKeynodes::concept_success_solution, solution);
  GenerationUtils::generateRelationBetween(
      ms_context, solution, outputStructure, InferenceKeynodes::nrel_output_structure);

  return solution;
}
