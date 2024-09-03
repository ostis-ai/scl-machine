/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DeleteSolutionAgent.hpp"

#include "keynodes/SolutionKeynodes.hpp"

#include "manager/DeleteSolutionManager.hpp"

namespace solutionModule
{
ScResult DeleteSolutionAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  ScAddr const & solution = action.GetArgument(1);
  try
  {
    if (m_context.IsElement(solution) == SC_FALSE)
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "DeleteSolutionAgent: solution is not valid");

    auto manager = std::make_unique<DeleteSolutionManager>(&m_context);
    manager->deleteSolution(solution);

    return action.FinishSuccessfully();
  }
  catch (utils::ScException const & exception)
  {
    SC_AGENT_LOG_ERROR(exception.Message());
    return action.FinishUnsuccessfully();
  }
}

ScAddr DeleteSolutionAgent::GetActionClass() const
{
  return SolutionKeynodes::action_delete_solution;
}
}  // namespace solutionModule
