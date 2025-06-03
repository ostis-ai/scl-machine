/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "EraseSolutionAgent.hpp"

#include "keynodes/SolutionKeynodes.hpp"

#include "manager/EraseSolutionManager.hpp"

namespace solutionModule
{

EraseSolutionAgent::EraseSolutionAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/EraseSolutionAgent.log", utils::ScLogLevel::Debug, true);
}

ScResult EraseSolutionAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  ScAddr const & solution = action.GetArgument(1);
  try
  {
    if (!m_context.IsElement(solution))
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, GetName() << ": solution is not valid");

    auto manager = std::make_unique<EraseSolutionManager>(&m_context, &m_logger);
    manager->eraseSolution(solution);

    return action.FinishSuccessfully();
  }
  catch (utils::ScException const & exception)
  {
    m_logger.Error(exception.Message());
    return action.FinishWithError();
  }
}

ScAddr EraseSolutionAgent::GetActionClass() const
{
  return SolutionKeynodes::action_erase_solution;
}
}  // namespace solutionModule
