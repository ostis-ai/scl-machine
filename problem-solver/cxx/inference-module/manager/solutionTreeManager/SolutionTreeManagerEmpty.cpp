/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "SolutionTreeManagerEmpty.hpp"

namespace inference
{
SolutionTreeManagerEmpty::SolutionTreeManagerEmpty(ScMemoryContext * context)
  : SolutionTreeManagerAbstract(context)
{
}

bool SolutionTreeManagerEmpty::addNode(ScAddr const & formula, Replacements const & replacements)
{
  return true;
}

}  // namespace inference
