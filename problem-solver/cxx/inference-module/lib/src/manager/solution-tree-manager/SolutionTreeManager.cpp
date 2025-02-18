/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "inference/replacements_utils.hpp"

#include "SolutionTreeManager.hpp"

#include "generator/SolutionTreeGenerator.hpp"

namespace inference
{
SolutionTreeManager::SolutionTreeManager(ScMemoryContext * context)
  : SolutionTreeManagerAbstract(context)
{
}

bool SolutionTreeManager::addNode(ScAddr const & formula, Replacements const & replacements)
{
  std::vector<ScTemplateParams> templateParamsVector;
  ReplacementsUtils::GetReplacementsToScTemplateParams(replacements, templateParamsVector);
  ScAddrUnorderedSet variables;
  ReplacementsUtils::GetKeySet(replacements, variables);
  bool result = true;
  for (ScTemplateParams const & templateParams : templateParamsVector)
    result &= solutionTreeGenerator->addNode(formula, templateParams, variables);
  return result;
}

}  // namespace inference
