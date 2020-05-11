/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "inferenceModule.hpp"
#include "keynodes/keynodes.hpp"

using namespace exampleModule;

SC_IMPLEMENT_MODULE(ExampleModule)

sc_result ExampleModule::InitializeImpl()
{
  if (!exampleModule::InferenceKeynodes::InitGlobal())
    return SC_RESULT_ERROR;

  return SC_RESULT_OK;
}

sc_result ExampleModule::ShutdownImpl()
{

  return SC_RESULT_OK;
}
