/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceModule.hpp"

#include "agent/DirectInferenceAgent.hpp"
#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

SC_IMPLEMENT_MODULE(InferenceModule)

sc_result InferenceModule::InitializeImpl()
{
  if (!InferenceKeynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(DirectInferenceAgent)

  return SC_RESULT_OK;
}

sc_result InferenceModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(DirectInferenceAgent)
  return SC_RESULT_OK;
}
