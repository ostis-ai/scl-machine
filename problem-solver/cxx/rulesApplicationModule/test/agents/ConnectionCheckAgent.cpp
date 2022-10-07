/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <sc-agents-common/utils/AgentUtils.hpp>
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "test/keynodes/TestKeynodes.hpp"

#include "ConnectionCheckAgent.hpp"

using namespace rulesApplicationModule;

SC_AGENT_IMPLEMENTATION(ConnectionCheckAgent)
{
  ScAddr actionAddr = m_memoryCtx.GetEdgeTarget(edgeAddr);
  if(!checkActionClass(actionAddr))
  {
    return SC_RESULT_OK;
  }
  SC_LOG_DEBUG("ConnectionCheckAgent started");

  ScAddr firstEquipmentPiece = utils::IteratorUtils::getFirstByOutRelation(
        &m_memoryCtx,
        actionAddr,
        scAgentsCommon::CoreKeynodes::rrel_1
  );
  ScAddr secondEquipmentPiece = utils::IteratorUtils::getFirstByOutRelation(
        &m_memoryCtx,
        actionAddr,
        scAgentsCommon::CoreKeynodes::rrel_2
  );
  ScAddr structure = utils::IteratorUtils::getFirstByOutRelation(
        &m_memoryCtx,
        actionAddr,
        scAgentsCommon::CoreKeynodes::rrel_3
  );

  if (isNodesConnected(firstEquipmentPiece, secondEquipmentPiece, structure))
  {
    markAsConnected(firstEquipmentPiece, secondEquipmentPiece, structure);
    SC_LOG_DEBUG("ConnectionCheckAgent: nodes are marked as connected");
  }

  SC_LOG_DEBUG("ConnectionCheckAgent finished");
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionAddr, true);
  return SC_RESULT_OK;
}

bool ConnectionCheckAgent::checkActionClass(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(
        TestKeynodes::action_check_connection,
        actionAddr,
        ScType::EdgeAccessConstPosPerm);
}

bool ConnectionCheckAgent::isNodesConnected(ScAddr const & first, ScAddr const & second, ScAddr const & structure)
{
  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        first,
        ScType::EdgeDCommonVar,
        ScType::NodeVar >> "_connector",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_connector);
  scTemplate.TripleWithRelation(
        second,
        ScType::EdgeDCommonVar,
        "_connector",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_connector);
  ScTemplateSearchResult searchResult;
  m_memoryCtx.HelperSearchTemplateInStruct(scTemplate, structure, searchResult);

  return !searchResult.IsEmpty();
}

void ConnectionCheckAgent::markAsConnected(ScAddr const & first, ScAddr const & second, ScAddr const & structure)
{
  ScAddr relationPair = m_memoryCtx.CreateEdge(ScType::EdgeUCommonConst, first, second);
  ScAddr relationAccessArc = m_memoryCtx.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        TestKeynodes::nrel_connected_elements,
        relationPair);

  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, relationPair);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, relationAccessArc);
  if(!m_memoryCtx.HelperCheckEdge(structure, TestKeynodes::nrel_connected_elements, ScType::EdgeAccessConstPosPerm))
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, TestKeynodes::nrel_connected_elements);
}
