/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/SolutionKeynodes.hpp"

#include "manager/DeleteSolutionManager.hpp"

#include "DeleteSolutionAgent.hpp"

namespace solutionModule
{
SC_AGENT_IMPLEMENTATION(DeleteSolutionAgent)
{
  ScAddr const & actionNode = otherAddr;
  if (checkActionClass(actionNode) == SC_FALSE)
    return SC_RESULT_OK;
  SC_LOG_INFO("DeleteSolutionAgent started");

  ScAddr const & solution =
      utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, scAgentsCommon::CoreKeynodes::rrel_1);
  try
  {
    if (m_memoryCtx.IsElement(solution) == SC_FALSE)
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "DeleteSolutionAgent: solution is not valid");

    auto manager = std::make_unique<DeleteSolutionManager>(&m_memoryCtx);
    manager->deleteSolution(solution);

    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, true);
    SC_LOG_INFO("DeleteSolutionAgent finished");
    return SC_RESULT_OK;
  }
  catch (utils::ScException const & exception)
  {
    SC_LOG_ERROR(exception.Message());
    SC_LOG_INFO("DeleteSolutionAgent finished with error");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, false);
    return SC_RESULT_ERROR;
  }
}

bool DeleteSolutionAgent::checkActionClass(ScAddr const & actionNode) const
{
  return m_memoryCtx.HelperCheckEdge(
      SolutionKeynodes::action_delete_solution, actionNode, ScType::EdgeAccessConstPosPerm);
}

}  // namespace solutionModule
