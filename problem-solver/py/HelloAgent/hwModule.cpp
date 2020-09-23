/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "hwModule.hpp"

SC_IMPLEMENT_MODULE(HelloAgentModule)

sc_result HelloAgentModule::InitializeImpl()
{
  m_hwService.reset(new HelloAgentPythonService("HelloAgent/HelloModule.py"));
  m_hwService->Run();
  return SC_RESULT_OK;
}

sc_result HelloAgentModule::ShutdownImpl()
{
  m_hwService->Stop();
  m_hwService.reset();
  return SC_RESULT_OK;
}
