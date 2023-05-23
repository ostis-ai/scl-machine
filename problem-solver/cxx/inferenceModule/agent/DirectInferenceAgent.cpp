/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

#include "DirectInferenceAgent.hpp"
#include "factory/InferenceManagerFactory.hpp"

using namespace scAgentsCommon;

namespace inference
{
SC_AGENT_IMPLEMENTATION(DirectInferenceAgent)
{
  if (!edgeAddr.IsValid())
    return SC_RESULT_ERROR;

  ScAddr actionNode = ms_context->GetEdgeTarget(edgeAddr);
  if (!checkActionClass(actionNode))
    return SC_RESULT_OK;

  SC_LOG_DEBUG("DirectInferenceAgent started");

  ScAddr const targetStructure =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_1);
  ScAddr const formulasSet =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_2);
  ScAddr const arguments =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_3);
  ScAddr const rrel_4 = utils::IteratorUtils::getRoleRelation(ms_context.get(), 4);
  ScAddr const inputStructure = utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, rrel_4);

  if (!targetStructure.IsValid() || !utils::IteratorUtils::getAnyFromSet(ms_context.get(), targetStructure).IsValid())
  {
    SC_LOG_WARNING("Target structure is not valid or empty.");
  }
  if (!formulasSet.IsValid() || !utils::IteratorUtils::getAnyFromSet(ms_context.get(), formulasSet).IsValid())
  {
    SC_LOG_ERROR("Formulas set is not valid or empty.");
    utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, false);
    return SC_RESULT_ERROR;
  }
  if (!arguments.IsValid())
  {
    SC_LOG_ERROR("Arguments are not valid.");
    utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, false);
    return SC_RESULT_ERROR;
  }

  SearchType templateSearcherType = SEARCH_IN_ALL_KB;
  ScAddrVector inputStructures;
  if (inputStructure.IsValid())
  {
    inputStructures.push_back(inputStructure);
    templateSearcherType = SEARCH_IN_STRUCTURES;
  }

  ScAddrVector answerElements;

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_FULL, templateSearcherType};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(ms_context.get(), arguments, ScType::Node);
  ScAddr const & outputStructure = ms_context->CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{
      formulasSet, argumentVector, inputStructures, outputStructure, targetStructure};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(ms_context.get(), inferenceConfig);
  bool targetAchieved;
  try
  {
    targetAchieved = inferenceManager->applyInference(inferenceParams);
  }
  catch (utils::ScException const & exception)
  {
    SC_LOG_ERROR(exception.Message());
    utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, false);
    return SC_RESULT_ERROR;
  }
  ScAddr solutionNode = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

  answerElements.push_back(solutionNode);
  utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, answerElements, true);
  SC_LOG_DEBUG("DirectInferenceAgent finished");
  return SC_RESULT_OK;
}

bool DirectInferenceAgent::checkActionClass(ScAddr const & actionNode)
{
  return ms_context->HelperCheckEdge(
      InferenceKeynodes::action_direct_inference, actionNode, ScType::EdgeAccessConstPosPerm);
}

}  // namespace inference
