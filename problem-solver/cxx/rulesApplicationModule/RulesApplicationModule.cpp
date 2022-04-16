/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "agent/RulesApplicationAgent.hpp"

#include "RulesApplicationModule.hpp"

using namespace rulesApplicationModule;

SC_IMPLEMENT_MODULE(RulesApplicationModule)

sc_result RulesApplicationModule::InitializeImpl()
{
  if (!Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(RulesApplicationAgent)

  return SC_RESULT_OK;
}

sc_result RulesApplicationModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(RulesApplicationAgent)

  return SC_RESULT_OK;
}
