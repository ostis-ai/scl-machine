/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "SolutionModule.hpp"

#include "agent/EraseSolutionAgent.hpp"

using namespace solutionModule;
SC_MODULE_REGISTER(SolutionModule)->Agent<EraseSolutionAgent>();
