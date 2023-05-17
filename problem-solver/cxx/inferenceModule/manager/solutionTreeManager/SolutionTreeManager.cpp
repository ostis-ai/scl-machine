/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "SolutionTreeManager.hpp"

namespace inference
{
SolutionTreeManager::SolutionTreeManager(ScMemoryContext * context)
  : SolutionTreeManagerAbstract(context)
{
}

bool SolutionTreeManager::addNode(ScAddr const & formula, Replacements const & replacements)
{
  std::vector<ScTemplateParams> const & templateParamsVector =
      ReplacementsUtils::getReplacementsToScTemplateParams(replacements);
  std::set<std::string> varNames;
  ReplacementsUtils::getKeySet(replacements, varNames);
  bool result = true;
  for (ScTemplateParams const & templateParams : templateParamsVector)
    result &= solutionTreeGenerator->addNode(formula, templateParams, varNames);
  return result;
}

}  // namespace inference
