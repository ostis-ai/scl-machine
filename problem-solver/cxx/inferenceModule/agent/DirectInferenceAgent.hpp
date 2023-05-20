/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "manager/inferenceManager/InferenceManagerAbstract.hpp"
#include "keynodes/InferenceKeynodes.hpp"

#include "DirectInferenceAgent.generated.hpp"

namespace inference
{
class DirectInferenceAgent : public ScAgent
{
  SC_CLASS(Agent, Event(InferenceKeynodes::action_direct_inference, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  static bool checkActionClass(ScAddr const & actionNode);
};

}  // namespace inference
