/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "manager/RuleApplicationManager.hpp"
#include "keynodes/Keynodes.hpp"

#include "RulesApplicationAgent.hpp"

using namespace rulesApplicationModule;
using namespace utils;
using namespace scAgentsCommon;

SC_AGENT_IMPLEMENTATION(RulesApplicationAgent)
{
  ScAddr actionAddr = otherAddr;
  if (!checkActionClass(actionAddr))
    return SC_RESULT_OK;
  SC_LOG_DEBUG("RulesApplicationAgent started");

  try
  {
    ScAddr rulesSet = IteratorUtils::getFirstByOutRelation(&m_memoryCtx, actionAddr, CoreKeynodes::rrel_1);
    ScAddr inputStructure = IteratorUtils::getFirstByOutRelation(&m_memoryCtx, actionAddr, CoreKeynodes::rrel_2);
    ScAddr resultStructure = IteratorUtils::getFirstByOutRelation(&m_memoryCtx, actionAddr, CoreKeynodes::rrel_3);
    if (!rulesSet.IsValid() || !inputStructure.IsValid() || !resultStructure.IsValid())
      throw std::runtime_error("RulesApplicationAgent: one or more action arguments not found.");
    RuleApplicationManager manager = RuleApplicationManager(&m_memoryCtx);
    manager.applyRules(rulesSet, inputStructure, resultStructure);

    SC_LOG_DEBUG("RulesApplicationAgent finished");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionAddr, resultStructure, true);
  }
  catch (std::exception & ex)
  {
    ScAddr answer = m_memoryCtx.CreateNode(ScType::NodeConstStruct);
    SC_LOG_ERROR(ex.what());
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionAddr, answer, false);
    return SC_RESULT_ERROR;
  }

  return SC_RESULT_OK;
}

bool RulesApplicationAgent::checkActionClass(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(
        Keynodes::action_apply_rules_on_the_model,
        actionAddr,
        ScType::EdgeAccessConstPosPerm);
}
