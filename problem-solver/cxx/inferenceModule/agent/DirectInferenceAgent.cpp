/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/
#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"

#include "DirectInferenceAgent.hpp"

using namespace std;
using namespace utils;
using namespace scAgentsCommon;

namespace inference
{

SC_AGENT_IMPLEMENTATION(DirectInferenceAgent)
{
  if (!edgeAddr.IsValid())
    return SC_RESULT_ERROR;

  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
  ScAddr targetTemplate = IteratorUtils::getFirstByOutRelation(ms_context.get(), questionNode, CoreKeynodes::rrel_1);
  ScAddr ruleSet = IteratorUtils::getFirstByOutRelation(ms_context.get(), questionNode, CoreKeynodes::rrel_2);
  ScAddr argumentSet = IteratorUtils::getFirstByOutRelation(ms_context.get(), questionNode, CoreKeynodes::rrel_3);

  if (!targetTemplate.IsValid() || !ruleSet.IsValid() || !argumentSet.IsValid())
  {
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }
  // TODO: Need to implement common logic of DI
  this->inferenceManager = new DirectInferenceManager(ms_context.get());
  ScAddr answer = this->inferenceManager->applyInference(targetTemplate, ruleSet, argumentSet);

  AgentUtils::finishAgentWork((ScMemoryContext *) ms_context.get(), questionNode, answer);

  delete this->inferenceManager;
  return SC_RESULT_OK;
}
}
