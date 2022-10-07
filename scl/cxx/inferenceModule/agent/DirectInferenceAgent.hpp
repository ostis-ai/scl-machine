/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/InferenceKeynodes.hpp"
#include "manager/DirectInferenceManager.hpp"

#include "DirectInferenceAgent.generated.hpp"

using namespace std;

namespace inference
{

class DirectInferenceAgent : public ScAgent
{
  SC_CLASS(Agent, Event(InferenceKeynodes::action_direct_inference, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  // TODO: Need to implement common logic of DI
  std::unique_ptr<DirectInferenceManager> inferenceManager;
};

}
