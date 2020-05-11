/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/kpm/sc_agent.hpp>
#include <sc-kpm/sc-agents-common/keynodes/CoreKeynodes.hpp>

#include "keynodes/InferenceKeynodes.hpp"

#include "ATemplateGeneration.generated.hpp"

namespace inference
{

class ATemplateGeneration : public ScAgent
{
  SC_CLASS(Agent, Event(InferenceKeynodes::question_use_logic_rule, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  void fillGenParams(ScAddr & paramsSet, ScTemplateParams & genParams);
};

}
