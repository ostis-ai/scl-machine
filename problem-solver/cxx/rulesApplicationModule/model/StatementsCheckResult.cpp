/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "StatementsCheckResult.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>

#include "searcher/RuleConstructionsSearcher.hpp"
#include "utils/RuleCheckResultUtils.hpp"

using namespace std;
using namespace rulesApplicationModule;
using namespace utils;

StatementsCheckResult::StatementsCheckResult() = default;

void StatementsCheckResult::add(
      ScMemoryContext * context,
      ScAddr const & atomicStatement,
      SearchWithContentResult const & searchResult,
      ScTemplateParams const & params)
{
  Replacements otherReplacements = createReplacements(context, atomicStatement, searchResult, params);
  addByReplacements(otherReplacements);
}

void StatementsCheckResult::add(
      ScMemoryContext * context,
      ScAddr const & atomicStatement,
      ScTemplateGenResult const & genResult)
{
  Replacements otherReplacements = createReplacements(context, atomicStatement, genResult);
  addByReplacements(otherReplacements);
}

void StatementsCheckResult::addByReplacements(Replacements const & otherReplacements)
{
  if (!replacements.empty() && !replacementsKeysIsEqual(replacements, otherReplacements))
    throw runtime_error("StatementsCheckResult: cannot add gen result - different set of variables.");
  size_t replacementsCombinationNumber = RuleCheckResultUtils::getReplacementCombinationsNumber(replacements);
  size_t otherReplacementsCombinationNumber = RuleCheckResultUtils::getReplacementCombinationsNumber(otherReplacements);
  size_t sizeToReserve = replacementsCombinationNumber + otherReplacementsCombinationNumber;
  for (auto const & otherReplacement : otherReplacements)
  {
    replacements[otherReplacement.first].reserve(sizeToReserve);
    replacements[otherReplacement.first].insert(
          replacements[otherReplacement.first].end(),
          otherReplacement.second.begin(),
          otherReplacement.second.end());
  }
}

bool StatementsCheckResult::replacementsKeysIsEqual(
      Replacements const & first,
      Replacements const & second)
{
  auto pred = [](auto a, auto b)
  { return a.first == b.first; };

  return first.size() == second.size()
         && equal(first.begin(), first.end(), second.begin(), pred);
}

bool StatementsCheckResult::extend(StatementsCheckResult const & other)
{
  notFoundStructures.insert(other.notFoundStructures.begin(), other.notFoundStructures.end());
  notSearchedStructures.insert(other.notSearchedStructures.begin(), other.notSearchedStructures.end());

  return update(other.replacements);
}

bool StatementsCheckResult::extend(
      ScMemoryContext * context,
      ScAddr const & atomicStatement,
      size_t const & nestingLevel,
      SearchWithContentResult const & searchResult,
      ScTemplateParams const & params) // replace with extend from ScTemplateSearchResult
{
  Replacements otherReplacements = createReplacements(context, atomicStatement, searchResult, params);
  if (otherReplacements.empty())
    notFoundStructures.insert({ atomicStatement, nestingLevel });

  return update(otherReplacements);
}

bool StatementsCheckResult::update(Replacements const & otherReplacements)
{
  bool result;
  vector<string> commonKeys = getCommonVariablesIdtf(otherReplacements);

  if (commonKeys.empty())
    result = updateWithoutKeys(otherReplacements);
  else
    result = updateWithCommonKeys(commonKeys, otherReplacements);

  return result;
}

Replacements StatementsCheckResult::getUniqueReplacements(vector<string> const & identifiers) const
{
  vector<string> existingKeys;
  for (auto const & identifier : identifiers)
  {
    if (replacements.find(identifier) != replacements.end())
      existingKeys.push_back(identifier);
  }

  Replacements replacementsSubset;
  size_t replacementsCombinationsNum = RuleCheckResultUtils::getReplacementCombinationsNumber(replacements);
  for (size_t checkedCombinationIndex = 0;
       checkedCombinationIndex < replacementsCombinationsNum; checkedCombinationIndex++)
  {
    size_t combinationToCompareIndex = checkedCombinationIndex + 1;
    bool isUnique = true;
    while (combinationToCompareIndex < replacementsCombinationsNum && isUnique)
    {
      isUnique = replacementCombinationsForKeysAreDifferent(
            checkedCombinationIndex,
            combinationToCompareIndex,
            existingKeys);
      combinationToCompareIndex++;
    }

    if (isUnique)
    {
      for (auto const & key : existingKeys)
        replacementsSubset[key].push_back(replacements.at(key)[checkedCombinationIndex]);
    }
  }

  return replacementsSubset;
}

// returns false if in replacements for every key in keys value[first]==value[second]
// returns true if there is key in keys that for this key value[first]!=value[second]
bool StatementsCheckResult::replacementCombinationsForKeysAreDifferent(
      size_t const & firstCombinationIndex,
      size_t const & secondCombinationIndex,
      vector<string> const & keys) const
{
  for (auto const & key : keys)
  {
    vector<ScAddr> value = replacements.at(key);
    if (value[firstCombinationIndex] != value[secondCombinationIndex])
      return true;
  }
  return false;
}

Replacements StatementsCheckResult::createReplacements(
      ScMemoryContext * context,
      ScAddr const & atomicStatement,
      ScTemplateGenResult const & genResult)
{
  Replacements otherReplacements;

  RuleConstructionsSearcher ruleConstructionsSearcher(context);
  vector<string> variableIdentifiers = ruleConstructionsSearcher.getVariablesNodesSystemIdentifiers(atomicStatement);
  for (auto const & identifier : variableIdentifiers)
    otherReplacements[identifier].push_back(genResult[identifier]);

  return otherReplacements;
}

Replacements StatementsCheckResult::createReplacements(
      ScMemoryContext * context,
      ScAddr const & atomicStatement,
      SearchWithContentResult const & searchResult,
      ScTemplateParams const & params)
{
  Replacements otherReplacements;
  if (!searchResult.empty())
  {
    RuleConstructionsSearcher ruleConstructionsSearcher(context);
    vector<string> variableIdentifiers = ruleConstructionsSearcher.getVariablesNodesSystemIdentifiers(atomicStatement);
    for (auto const & searchResultItem : searchResult)
    {
      for (auto const & identifier : variableIdentifiers)
      {
        ScAddr replacement;
        if (params.Get(identifier, replacement))
          otherReplacements[identifier].push_back(replacement);
        else
          otherReplacements[identifier].push_back(searchResultItem[identifier]);
      }
    }
  }

  return otherReplacements;
}

vector<string> StatementsCheckResult::getCommonVariablesIdtf(Replacements const & otherReplacements)
{
  vector<string> commonKeys;
  for (auto const & replacement : replacements)
  {
    auto const & otherIt = otherReplacements.find(replacement.first);
    if (otherIt != otherReplacements.end())
    {
      commonKeys.push_back(replacement.first);
    }
  }
  return commonKeys;
}

bool StatementsCheckResult::updateWithoutKeys(Replacements const & otherReplacements)
{
  bool result = false;

  if (replacements.empty())
  {
    replacements = otherReplacements;
    result = true;
  }
  else if (!otherReplacements.empty())
  {
    Replacements updatedReplacements;

    size_t replacementsCombinationsNum = RuleCheckResultUtils::getReplacementCombinationsNumber(replacements);
    size_t otherReplacementsCombinationsNum = RuleCheckResultUtils::getReplacementCombinationsNumber(otherReplacements);
    for (size_t replacementIndex = 0; replacementIndex < replacementsCombinationsNum; replacementIndex++)
    {
      for (size_t otherReplacementIndex = 0;
           otherReplacementIndex < otherReplacementsCombinationsNum; otherReplacementIndex++)
      {
        for (auto const & pair : replacements)
          updatedReplacements[pair.first].push_back(pair.second[replacementIndex]);
        for (auto const & pair : otherReplacements)
          updatedReplacements[pair.first].push_back(pair.second[otherReplacementIndex]);
      }
    }
    replacements = updatedReplacements;
    result = true;
  }

  return result;
}

bool StatementsCheckResult::updateWithCommonKeys(
      std::vector<std::string> const & commonKeys,
      Replacements const & otherReplacements)
{
  bool result = false;
  Replacements updatedReplacements;

  size_t replacementsSize = RuleCheckResultUtils::getReplacementCombinationsNumber(replacements);
  size_t otherReplacementsSize = RuleCheckResultUtils::getReplacementCombinationsNumber(otherReplacements);
  vector<string> additionalKeys = getAdditionalKeys(otherReplacements);

  for (size_t replacementIndex = 0; replacementIndex < replacementsSize; replacementIndex++)
  {
    for (size_t otherReplacementIndex = 0; otherReplacementIndex < otherReplacementsSize; otherReplacementIndex++)
    {
      bool commonReplacementsAreEqual = commonKeysInReplacementsCombinationAreEqual(
            commonKeys,
            otherReplacements,
            otherReplacementIndex,
            replacementIndex);
      if (commonReplacementsAreEqual)
      {
        for (auto const & pair : replacements)
          updatedReplacements[pair.first].push_back(pair.second[replacementIndex]);
        for (auto const & additionalKey : additionalKeys)
          updatedReplacements[additionalKey].push_back(otherReplacements.at(additionalKey)[otherReplacementIndex]);
      }
    }
  }
  // All common variable replacements are different - the two structures are independent
  // The structure we found is not a continuation of the previous one - nothing to merge.
  if (!updatedReplacements.empty())
  {
    replacements = updatedReplacements;
    result = true;
  }

  return result;
}

bool StatementsCheckResult::commonKeysInReplacementsCombinationAreEqual(
      std::vector<std::string> const & commonKeys,
      Replacements const & otherReplacements,
      size_t const & otherReplacementsCombinationIndex,
      size_t const & replacementsCombinationIndex)
{
  for (auto & commonKey: commonKeys)
  {
    if (replacements[commonKey][replacementsCombinationIndex] !=
        otherReplacements.at(commonKey)[otherReplacementsCombinationIndex])
      return false;
  }

  return true;
}

vector<string> StatementsCheckResult::getAdditionalKeys(Replacements const & otherReplacements)
{
  vector<string> additionalKeys;
  for (auto const & pair : otherReplacements)
  {
    if (replacements.find(pair.first) == replacements.end())
      additionalKeys.push_back(pair.first);
  }

  return additionalKeys;
}

void StatementsCheckResult::printReplacementsInDebug(ScMemoryContext * context) const
{
  SC_LOG_DEBUG("              Replacements")
  for (auto const & pair : replacements)
  {
    SC_LOG_DEBUG("++++ for " + pair.first)
    for (auto const & addr : pair.second)
    {
      SC_LOG_DEBUG("------- " + context->HelperGetSystemIdtf(addr))
    }
  }
}
