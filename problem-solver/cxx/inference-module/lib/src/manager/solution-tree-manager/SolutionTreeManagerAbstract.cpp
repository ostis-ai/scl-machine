/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "inference/solution_tree_manager_abstract.hpp"

#include "generator/SolutionTreeGenerator.hpp"
#include "searcher/solution-tree-searcher/SolutionTreeSearcher.hpp"

namespace inference
{
SolutionTreeManagerAbstract::SolutionTreeManagerAbstract(ScMemoryContext * context)
{
  solutionTreeGenerator = std::make_unique<SolutionTreeGenerator>(context);
  solutionTreeSearcher = std::make_unique<SolutionTreeSearcher>(context);
}

SolutionTreeManagerAbstract::~SolutionTreeManagerAbstract() = default;

ScAddr SolutionTreeManagerAbstract::generateSolution(ScAddr const & outputStructure, bool targetAchieved)
{
  return solutionTreeGenerator->generateSolution(outputStructure, targetAchieved);
}

bool SolutionTreeManagerAbstract::checkIfSolutionNodeExists(
    ScAddr const & formula,
    ScTemplateParams const & templateParams,
    ScAddrUnorderedSet const & variables)
{
  return solutionTreeSearcher->checkIfSolutionNodeExists(formula, templateParams, variables);
}
}  // namespace inference
