/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceManagerGeneral.hpp"

#include "strategy/FormulasIterationStrategyTarget.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>

using namespace inference;

InferenceManagerGeneral::InferenceManagerGeneral(ScMemoryContext * ms_context)
  : ms_context(ms_context) { }

void InferenceManagerGeneral::setFormulasIterationStrategy(std::unique_ptr<FormulasIterationStrategyAbstract> strategy)
{
  formulasIterationStrategy = std::move(strategy);
}

/**
 * @brief Use formulasIterationStrategy to apply formulas using configured strategy
 * @param formulasSet is an oriented set of formulas sets to apply
 * @returns ScAddr of the solution node, connected with output structure
*/
ScAddr InferenceManagerGeneral::applyInference(ScAddr const & formulasSet)
{
  ScAddr const & outputStructure = ms_context->CreateNode(ScType::NodeConstStruct);

  bool const targetAchieved = formulasIterationStrategy->applyIterationStrategy(formulasSet, outputStructure);

  return formulasIterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);
}
