/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "agent/DeleteSolutionAgent.hpp"

#include "keynodes/SolutionKeynodes.hpp"

#include "SolutionModule.hpp"

using namespace solutionModule;
SC_IMPLEMENT_MODULE(SolutionModule)

sc_result SolutionModule::InitializeImpl()
{
  if (SolutionKeynodes::InitGlobal() == SC_FALSE)
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(DeleteSolutionAgent)

  return SC_RESULT_OK;
}

sc_result SolutionModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(DeleteSolutionAgent)
  return SC_RESULT_OK;
}
