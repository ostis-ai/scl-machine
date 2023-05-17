/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "generator/SolutionTreeGenerator.hpp"
#include "searcher/solutionTreeSearcher/SolutionTreeSearcher.hpp"
#include "utils/ReplacementsUtils.hpp"

namespace inference
{
/// Class to create solution tree of the inference
class SolutionTreeManagerAbstract
{
public:
  explicit SolutionTreeManagerAbstract(ScMemoryContext * context);

  virtual ~SolutionTreeManagerAbstract() = default;

  virtual bool addNode(ScAddr const & formula, Replacements const & replacements) = 0;

  ScAddr createSolution(ScAddr const & outputStructure, bool targetAchieved);

  bool checkIfSolutionNodeExists(
      ScAddr const & formula,
      ScTemplateParams const & templateParams,
      std::set<std::string> const & varNames);

protected:
  std::unique_ptr<SolutionTreeGenerator> solutionTreeGenerator;
  std::unique_ptr<SolutionTreeSearcher> solutionTreeSearcher;
};

}  // namespace inference
