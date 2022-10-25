/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/InferenceKeynodes.hpp"

#include "AReverseInference.generated.hpp"

using namespace std;

namespace inference
{

class AReverseInference : public ScAgent
{
  SC_CLASS(Agent, Event(InferenceKeynodes::action_reverse_inference, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  bool generateStatement(ScAddr & statement, const ScTemplateParams & templateParams);

  void createTemplateParams(ScAddr & statement, vector<ScAddr> argumentList, ScTemplateParams & templateParam);

  vector<ScAddr> getRequestKeyElementList(ScAddr & request);

  vector<ScAddr> findRulesWithKeyElement(ScAddr & keyElement);
};

}  // namespace inference
