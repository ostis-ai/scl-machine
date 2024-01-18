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

  ScAddr actionNode = m_memoryCtx.GetEdgeTarget(edgeAddr);
  if (!checkActionClass(&m_memoryCtx, actionNode))
    return SC_RESULT_OK;

  SC_LOG_DEBUG("DirectInferenceAgent started");

  ScAddr const targetStructure =
      utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, CoreKeynodes::rrel_1);
  ScAddr const formulasSet = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, CoreKeynodes::rrel_2);
  ScAddr const arguments = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, CoreKeynodes::rrel_3);
  ScAddr const rrel_4 = utils::IteratorUtils::getRoleRelation(&m_memoryCtx, 4);
  ScAddr const inputStructure = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, rrel_4);

  if (!targetStructure.IsValid() || !utils::IteratorUtils::getAnyFromSet(&m_memoryCtx, targetStructure).IsValid())
  {
    SC_LOG_WARNING("Target structure is not valid or empty.");
  }
  if (!formulasSet.IsValid() || !utils::IteratorUtils::getAnyFromSet(&m_memoryCtx, formulasSet).IsValid())
  {
    SC_LOG_ERROR("Formulas set is not valid or empty.");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, false);
    return SC_RESULT_ERROR;
  }
  if (!arguments.IsValid())
  {
    SC_LOG_ERROR("Arguments are not valid.");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, false);
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
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&m_memoryCtx, arguments, ScType::Node);
  ScAddr const & outputStructure = m_memoryCtx.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{
      formulasSet, argumentVector, inputStructures, outputStructure, targetStructure};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&m_memoryCtx, inferenceConfig);
  bool targetAchieved;
  try
  {
    targetAchieved = inferenceManager->applyInference(inferenceParams);
  }
  catch (utils::ScException const & exception)
  {
    SC_LOG_ERROR(exception.Message());
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, false);
    return SC_RESULT_ERROR;
  }
  ScAddr solutionNode = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

  answerElements.push_back(solutionNode);
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, answerElements, true);
  SC_LOG_DEBUG("DirectInferenceAgent finished");
  return SC_RESULT_OK;
}

bool DirectInferenceAgent::checkActionClass(ScMemoryContext * context, ScAddr const & actionNode)
{
  return context->HelperCheckEdge(
      InferenceKeynodes::action_direct_inference, actionNode, ScType::EdgeAccessConstPosPerm);
}

}  // namespace inference
