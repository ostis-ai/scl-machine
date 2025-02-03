/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "SolutionTreeGenerator.hpp"

#include "keynodes/InferenceKeynodes.hpp"

#include <sc-agents-common/utils/GenerationUtils.hpp>

#include <sc-memory/sc_addr.hpp>

using namespace inference;
using namespace utils;
SolutionTreeGenerator::SolutionTreeGenerator(ScMemoryContext * ms_context)
  : ms_context(ms_context)
{
  solution = ms_context->GenerateNode(ScType::ConstNode);
  ms_context->GenerateConnector(ScType::ConstPermPosArc, InferenceKeynodes::concept_solution, solution);
}

bool SolutionTreeGenerator::addNode(
    ScAddr const & formula,
    ScTemplateParams const & templateParams,
    ScAddrUnorderedSet const & variables)
{
  ScAddr newSolutionNode = generateSolutionNode(formula, templateParams, variables);
  bool result = newSolutionNode.IsValid();
  if (result)
  {
    if (!lastSolutionNode.IsValid())
    {
      result = GenerationUtils::generateRelationBetween(ms_context, solution, newSolutionNode, ScKeynodes::rrel_1);
    }
    else
    {
      ScIterator3Ptr lastSolutionNodeArcIterator =
          ms_context->CreateIterator3(solution, ScType::ConstPermPosArc, lastSolutionNode);
      if (lastSolutionNodeArcIterator->Next())
      {
        ScAddr lastSolutionNodeArc = lastSolutionNodeArcIterator->Get(1);
        ScAddr newSolutionNodeArc = ms_context->GenerateConnector(ScType::ConstPermPosArc, solution, newSolutionNode);
        GenerationUtils::generateRelationBetween(
            ms_context, lastSolutionNodeArc, newSolutionNodeArc, ScKeynodes::nrel_basic_sequence);
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

ScAddr SolutionTreeGenerator::generateSolutionNode(
    ScAddr const & formula,
    ScTemplateParams const & templateParams,
    ScAddrUnorderedSet const & variables)
{
  ScAddr const & solutionNode = ms_context->GenerateNode(ScType::ConstNode);
  GenerationUtils::generateRelationBetween(ms_context, solutionNode, formula, ScKeynodes::rrel_1);
  ScAddr const & replacementsNode = ms_context->GenerateNode(ScType::ConstNode);
  GenerationUtils::generateRelationBetween(ms_context, solutionNode, replacementsNode, ScKeynodes::rrel_2);
  for (ScAddr const & variable : variables)
  {
    ScAddr replacement;
    templateParams.Get(variable, replacement);
    if (replacement.IsValid())
    {
      ScAddr const & pair = ms_context->GenerateNode(ScType::ConstNode);
      ms_context->GenerateConnector(ScType::ConstPermPosArc, replacementsNode, pair);
      GenerationUtils::generateRelationBetween(ms_context, pair, replacement, ScKeynodes::rrel_1);
      GenerationUtils::generateRelationBetween(ms_context, pair, variable, ScKeynodes::rrel_2);
      ms_context->GenerateConnector(ScType::ConstTempPosArc, variable, replacement);
    }
    else
      SC_THROW_EXCEPTION(
          utils::ExceptionItemNotFound,
          "SolutionTreeGenerator: formula " << ms_context->GetElementSystemIdentifier(formula) << " has var "
                                            << ms_context->GetElementSystemIdentifier(variable)
                                            << " but scTemplateParams don't have replacement for this var");
  }

  return solutionNode;
}

ScAddr SolutionTreeGenerator::generateSolution(ScAddr const & outputStructure, bool targetAchieved)
{
  ScType arcType = targetAchieved ? ScType::ConstPermPosArc : ScType::ConstPermNegArc;
  ms_context->GenerateConnector(arcType, InferenceKeynodes::concept_success_solution, solution);
  GenerationUtils::generateRelationBetween(
      ms_context, solution, outputStructure, InferenceKeynodes::nrel_output_structure);

  return solution;
}
