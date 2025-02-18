/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

#include "inference/types.hpp"

namespace inference
{
class SolutionTreeGenerator;
class SolutionTreeSearcher;

/// Class to generate solution tree of the inference
class SolutionTreeManagerAbstract
{
public:
  explicit SolutionTreeManagerAbstract(ScMemoryContext * context);

  virtual ~SolutionTreeManagerAbstract();

  virtual bool AddNode(ScAddr const & formula, Replacements const & replacements) = 0;

  ScAddr GenerateSolution(ScAddr const & outputStructure, bool targetAchieved);

  bool CheckIfSolutionNodeExists(
      ScAddr const & formula,
      ScTemplateParams const & templateParams,
      ScAddrUnorderedSet const & variables);

protected:
  std::unique_ptr<SolutionTreeGenerator> solutionTreeGenerator;
  std::unique_ptr<SolutionTreeSearcher> solutionTreeSearcher;
};

}  // namespace inference
