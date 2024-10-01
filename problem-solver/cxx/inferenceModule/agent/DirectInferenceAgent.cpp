/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DirectInferenceAgent.hpp"

#include "factory/InferenceManagerFactory.hpp"

#include "keynodes/InferenceKeynodes.hpp"

#include <sc-agents-common/utils/IteratorUtils.hpp>

namespace inference
{
ScResult DirectInferenceAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  auto const [targetStructure, formulasSet, arguments, inputStructure] = action.GetArguments<4>();

  if (!IsSetValidAndNotEmpty(targetStructure))
  {
    SC_AGENT_LOG_WARNING("Target structure is not valid or empty.");
  }
  if (!IsSetValidAndNotEmpty(formulasSet))
  {
    SC_AGENT_LOG_ERROR("Formulas set is not valid or empty.");
    return action.FinishUnsuccessfully();
  }
  if (!arguments.IsValid())
  {
    SC_AGENT_LOG_ERROR("Arguments are not valid.");
    return action.FinishUnsuccessfully();
  }

  SearchType templateSearcherType = SEARCH_IN_ALL_KB;
  ScAddrUnorderedSet inputStructures;
  if (inputStructure.IsValid())
  {
    inputStructures.insert(inputStructure);
    templateSearcherType = SEARCH_IN_STRUCTURES;
  }

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_FULL, templateSearcherType};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&m_context, arguments, ScType::Node);
  ScAddr const & outputStructure = m_context.GenerateNode(ScType::ConstNodeStructure);
  InferenceParams const & inferenceParams{
      formulasSet, argumentVector, inputStructures, outputStructure, targetStructure};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&m_context, inferenceConfig);
  bool targetAchieved;
  try
  {
    targetAchieved = inferenceManager->applyInference(inferenceParams);
  }
  catch (utils::ScException const & exception)
  {
    SC_AGENT_LOG_ERROR(exception.Message());
    return action.FinishUnsuccessfully();
  }
  ScAddr solutionNode = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

  action.FormResult(solutionNode);
  return action.FinishSuccessfully();
}

ScAddr DirectInferenceAgent::GetActionClass() const
{
  return InferenceKeynodes::action_direct_inference;
}

bool DirectInferenceAgent::IsSetValidAndNotEmpty(ScAddr const & setAddr) const
{
  if (!setAddr.IsValid())
    return false;
  ScSet const & scSet = m_context.ConvertToSet(setAddr);
  return !scSet.IsEmpty();
}
}  // namespace inference
