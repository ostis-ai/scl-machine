/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <queue>
#include <vector>

#include <sc-memory/kpm/sc_agent.hpp>

namespace inference
{
class SolutionTreeGenerator
{
public:
  explicit SolutionTreeGenerator(ScMemoryContext * ms_context);

  ~SolutionTreeGenerator() = default;

  bool addNode(ScAddr const & formula, ScTemplateParams const & templateParams, std::set<std::string> const & varNames);

  ScAddr createSolution(ScAddr const & outputStructure, bool targetAchieved);

private:
  ScAddr createSolutionNode(
      ScAddr const & formula,
      ScTemplateParams const & templateParams,
      std::set<std::string> const & varNames);

  ScMemoryContext * ms_context;
  ScAddr solution;
  ScAddr lastSolutionNode;
};

}  // namespace inference
