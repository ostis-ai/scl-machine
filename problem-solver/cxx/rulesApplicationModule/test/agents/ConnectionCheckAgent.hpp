/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "ConnectionCheckAgent.generated.hpp"

namespace rulesApplicationModule
{

class ConnectionCheckAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  bool checkActionClass(ScAddr const & actionAddr);

  bool isNodesConnected(ScAddr const & first, ScAddr const & second, ScAddr const & structure);

  void markAsConnected(ScAddr const & first, ScAddr const & second, ScAddr const & structure);
};

}
