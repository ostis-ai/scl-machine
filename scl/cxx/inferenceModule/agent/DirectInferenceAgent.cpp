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
  SC_LOG_DEBUG("Starting DirectInferenceAgent");
  if (!edgeAddr.IsValid())
    return SC_RESULT_ERROR;

  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
  ScAddr rrel_4 = ms_context->HelperResolveSystemIdtf("rrel_4");
  ScAddr ruleSet = utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), questionNode, CoreKeynodes::rrel_1);
  ScAddr inputStructure =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), questionNode, CoreKeynodes::rrel_2);
  ScAddr outputStructure =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), questionNode, CoreKeynodes::rrel_3);
  ScAddr targetTemplate = utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), questionNode, rrel_4);

  if (!ruleSet.IsValid())
    SC_LOG_WARNING("Rule set is not valid");
  if (!inputStructure.IsValid())
    SC_LOG_WARNING("Input structure is not valid");
  if (!outputStructure.IsValid())
    SC_LOG_WARNING("Output structure is not valid");
  if (!targetTemplate.IsValid())
    SC_LOG_WARNING("Target template is not valid");

  this->inferenceManager = std::make_unique<DirectInferenceManager>(ms_context.get());
  ScAddrVector answers;
  ScAddr answer = this->inferenceManager->applyInference(ruleSet, inputStructure, outputStructure, targetTemplate);
  answers.push_back(answer);

  bool success =
      ms_context->HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm);
  utils::AgentUtils::finishAgentWork((ScMemoryContext *)ms_context.get(), questionNode, answers, success);

  return SC_RESULT_OK;
}
}  // namespace inference
