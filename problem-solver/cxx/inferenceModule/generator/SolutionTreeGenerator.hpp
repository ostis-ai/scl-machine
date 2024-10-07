/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "utils/Types.hpp"

#include <sc-memory/sc_agent.hpp>

#include <vector>
#include <queue>

namespace inference
{
class SolutionTreeGenerator
{
public:
  explicit SolutionTreeGenerator(ScMemoryContext * ms_context);

  ~SolutionTreeGenerator() = default;

  bool addNode(ScAddr const & formula, ScTemplateParams const & templateParams, ScAddrUnorderedSet const & variables);

  ScAddr generateSolution(ScAddr const & outputStructure, bool targetAchieved);

private:
  ScAddr generateSolutionNode(
      ScAddr const & formula,
      ScTemplateParams const & templateParams,
      ScAddrUnorderedSet const & variables);

  ScMemoryContext * ms_context;
  ScAddr solution;
  ScAddr lastSolutionNode;
};

}  // namespace inference
