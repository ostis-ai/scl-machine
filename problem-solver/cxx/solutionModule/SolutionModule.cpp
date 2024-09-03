/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "agent/DeleteSolutionAgent.hpp"

#include "keynodes/SolutionKeynodes.hpp"

#include "SolutionModule.hpp"

using namespace solutionModule;
SC_MODULE_REGISTER(SolutionModule)->Agent<DeleteSolutionAgent>();
