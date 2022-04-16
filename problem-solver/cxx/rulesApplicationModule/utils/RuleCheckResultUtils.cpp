/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "utils/RuleCheckResultUtils.hpp"

namespace rulesApplicationModule
{

size_t RuleCheckResultUtils::getReplacementCombinationsNumber(const Replacements & replacements)
{
  size_t numberOfCombinations;
  replacements.empty() ? numberOfCombinations = 0 : numberOfCombinations = replacements.begin()->second.size();
  return numberOfCombinations;
}

}
