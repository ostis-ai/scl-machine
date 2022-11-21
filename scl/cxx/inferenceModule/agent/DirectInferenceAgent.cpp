/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

#include "keynodes/InferenceKeynodes.hpp"

#include "DirectInferenceAgent.hpp"

using namespace std;
using namespace scAgentsCommon;

namespace inference
{

SC_AGENT_IMPLEMENTATION(DirectInferenceAgent)
{
  if (!edgeAddr.IsValid())
    return SC_RESULT_ERROR;

  ScAddr actionNode = m_memoryCtx.GetEdgeTarget(edgeAddr);
  if (!checkActionClass(actionNode))
    return SC_RESULT_OK;

  SC_LOG_DEBUG("DirectInferenceAgent started");

  ScAddr targetStructure = utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_1);
  ScAddr formulasSet = utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_2);
  ScAddr inputStructure =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_3);
  ScAddr rrel_4 = utils::IteratorUtils::getRoleRelation(ms_context.get(), 4);
  ScAddr outputStructure =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, rrel_4);

  if (!targetStructure.IsValid())
    SC_LOG_WARNING("Target structure is not valid");
  if (!formulasSet.IsValid())
    SC_LOG_WARNING("Formulas set is not valid");
  if (!inputStructure.IsValid())
    SC_LOG_WARNING("Input structure is not valid");
  if (!outputStructure.IsValid())
  {
    SC_LOG_WARNING("Output structure is not valid, generate new");
    outputStructure = ms_context->CreateNode(ScType::NodeConstStruct);
  }

  this->inferenceManager = std::make_unique<DirectInferenceManager>(ms_context.get());
  ScAddr solutionNode;
  solutionNode = this->inferenceManager->applyInference(targetStructure, formulasSet, inputStructure, outputStructure);
  ScAddrVector answerElements;
  answerElements.push_back(solutionNode);

  bool isSuccess =
      ms_context->HelperCheckEdge(InferenceKeynodes::concept_success_solution, solutionNode, ScType::EdgeAccessConstPosPerm);

  utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, answerElements, isSuccess);
  SC_LOG_DEBUG("DirectInferenceAgent finished");
  return SC_RESULT_OK;
}

bool DirectInferenceAgent::checkActionClass(ScAddr const & actionNode)
{
  return m_memoryCtx.HelperCheckEdge(
        InferenceKeynodes::action_direct_inference, actionNode, ScType::EdgeAccessConstPosPerm);
}

}  // namespace inference
