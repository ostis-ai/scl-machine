/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "SolutionTreeManagerAbstract.hpp"

namespace inference
{
SolutionTreeManagerAbstract::SolutionTreeManagerAbstract(ScMemoryContext * context)
{
  solutionTreeGenerator = std::make_unique<SolutionTreeGenerator>(context);
  solutionTreeSearcher = std::make_unique<SolutionTreeSearcher>(context);
}

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
