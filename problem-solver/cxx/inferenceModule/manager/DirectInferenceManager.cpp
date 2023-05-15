/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DirectInferenceManager.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "strategy/FormulasIterationStrategyAbstract.hpp"
#include "director/InferenceManagerDirector.hpp"

using namespace inference;

DirectInferenceManager::DirectInferenceManager(ScMemoryContext * ms_context)
  : ms_context(ms_context)
{
}

ScAddr DirectInferenceManager::applyInference(
    ScAddr const & targetStructure,
    ScAddr const & formulasSet,
    ScAddr const & arguments,
    ScAddr const & inputStructure)
{
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(ms_context, arguments, ScType::Node);
  ScAddrVector inputStructures;
  if (inputStructure.IsValid())
  {
    inputStructures.push_back(inputStructure);
  }
  InferenceFlowConfig const & inferenceFlowConfig {true, true, false};
  std::unique_ptr<FormulasIterationStrategyAbstract> strategy =
      InferenceManagerDirector::constructDirectInferenceManagerTarget(
          ms_context, inferenceFlowConfig, inputStructures);

  ScAddr const & outputStructure = ms_context->CreateNode(ScType::NodeConstStruct);
  InferenceParamsConfig const & inferenceParamsConfig {formulasSet, argumentVector, inputStructures, outputStructure, targetStructure};
  bool const targetAchieved = strategy->applyIterationStrategy(inferenceParamsConfig);

  return strategy->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);
}
