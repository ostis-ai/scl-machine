/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ReplacementsUtils.hpp"
#include "sc-memory/sc_agent.hpp"
namespace inference
{

void ReplacementsUtils::intersectReplacements(
    Replacements const & first,
    Replacements const & second,
    Replacements & intersection)
{
  std::vector<std::pair<size_t, size_t>> firstSecondPairs;
  ScAddrUnorderedSet firstKeys;
  getKeySet(first, firstKeys);
  ScAddrUnorderedSet secondKeys;
  getKeySet(second, secondKeys);
  size_t firstAmountOfColumns = getColumnsAmount(first);
  size_t secondAmountOfColumns = getColumnsAmount(second);

  if (firstAmountOfColumns == 0)
  {
    intersection = second;
    return;
  }
  if (secondAmountOfColumns == 0)
  {
    intersection = first;
    return;
  }

  ScAddrUnorderedSet commonKeysSet;
  getCommonKeys(firstKeys, secondKeys, commonKeysSet);

  ReplacementsHashes firstHashes;
  calculateHashesForCommonKeys(first, commonKeysSet, firstHashes);
  ReplacementsHashes secondHashes;
  calculateHashesForCommonKeys(second, commonKeysSet, secondHashes);
  for (auto const & firstHashPair : firstHashes)
  {
    auto const & secondHashPairIterator = secondHashes.find(firstHashPair.first);
    if (secondHashPairIterator == secondHashes.cend())
      continue;
    for (auto const & columnIndexInFirst : firstHashPair.second)
    {
      for (auto const & columnIndexInSecond : secondHashPairIterator->second)
      {
        bool commonPartsAreIdentical = true;
        for (ScAddr const & commonKey : commonKeysSet)
        {
          if (first.at(commonKey).at(columnIndexInFirst) != second.at(commonKey).at(columnIndexInSecond))
          {
            commonPartsAreIdentical = false;
            break;
          }
        }
        if (commonPartsAreIdentical)
          firstSecondPairs.emplace_back(columnIndexInFirst, columnIndexInSecond);
      }
    }
  }

  Replacements result;
  for (ScAddr const & firstKey : firstKeys)
    result[firstKey].reserve(firstSecondPairs.size());
  for (ScAddr const & secondKey : secondKeys)
  {
    if (!commonKeysSet.count(secondKey))
      result[secondKey].reserve(firstSecondPairs.size());
  }

  for (auto const & firstSecondPair : firstSecondPairs)
  {
    for (ScAddr const & firstKey : firstKeys)
      result.at(firstKey).push_back(first.at(firstKey).at(firstSecondPair.first));
    for (ScAddr const & secondKey : secondKeys)
    {
      if (!commonKeysSet.count(secondKey))
        result.at(secondKey).push_back(second.at(secondKey).at(firstSecondPair.second));
    }
  }
  removeDuplicateColumns(result);
  intersection = result;
}

void ReplacementsUtils::subtractReplacements(
    Replacements const & first,
    Replacements const & second,
    Replacements & difference)
{
  ScAddrUnorderedSet firstKeys;
  getKeySet(first, firstKeys);
  ScAddrUnorderedSet secondKeys;
  getKeySet(second, secondKeys);
  size_t firstAmountOfColumns = getColumnsAmount(first);
  size_t secondAmountOfColumns = getColumnsAmount(second);
  std::vector<size_t> firstColumns;
  firstColumns.reserve(firstAmountOfColumns);

  if (firstAmountOfColumns == 0 || secondAmountOfColumns == 0)
  {
    difference = first;
    return;
  }

  ScAddrUnorderedSet commonKeysSet;
  getCommonKeys(firstKeys, secondKeys, commonKeysSet);

  if (commonKeysSet.empty())
  {
    difference = first;
    return;
  }

  ReplacementsHashes firstHashes;
  calculateHashesForCommonKeys(first, commonKeysSet, firstHashes);
  ReplacementsHashes secondHashes;
  calculateHashesForCommonKeys(second, commonKeysSet, secondHashes);
  for (auto const & firstHashPair : firstHashes)
  {
    auto const & secondHashPairIterator = secondHashes.find(firstHashPair.first);
    if (secondHashPairIterator == secondHashes.cend())
    {
      firstColumns.insert(firstColumns.end(), firstHashPair.second.cbegin(), firstHashPair.second.cend());
      continue;
    }
    for (auto const & columnIndexInFirst : firstHashPair.second)
    {
      bool hasPairWithSimilarValues = false;
      for (auto const & columnIndexInSecond : secondHashPairIterator->second)
      {
        bool hasDifferentValuesForAnyKey = false;
        for (ScAddr const & commonKey : commonKeysSet)
        {
          if (first.at(commonKey).at(columnIndexInFirst) != second.at(commonKey).at(columnIndexInSecond))
          {
            hasDifferentValuesForAnyKey = true;
            break;
          }
        }
        hasPairWithSimilarValues = !hasDifferentValuesForAnyKey;
        if (hasPairWithSimilarValues)
          break;
      }
      if (!hasPairWithSimilarValues)
        firstColumns.push_back(columnIndexInFirst);
    }
  }

  Replacements result;
  for (ScAddr const & firstKey : firstKeys)
    result[firstKey].reserve(firstColumns.size());

  for (auto const & firstColumn : firstColumns)
  {
    for (ScAddr const & firstKey : firstKeys)
      result.at(firstKey).push_back(first.at(firstKey).at(firstColumn));
  }
  removeDuplicateColumns(result);
  difference = result;
}

void ReplacementsUtils::uniteReplacements(
    Replacements const & first,
    Replacements const & second,
    Replacements & unionResult)
{
  ScAddrUnorderedSet firstKeys;
  getKeySet(first, firstKeys);
  ScAddrUnorderedSet secondKeys;
  getKeySet(second, secondKeys);
  ScAddrUnorderedSet commonKeysSet;
  getCommonKeys(firstKeys, secondKeys, commonKeysSet);
  size_t firstAmountOfColumns = getColumnsAmount(first);
  if (firstAmountOfColumns == 0)
  {
    unionResult = second;
    return;
  }
  size_t secondAmountOfColumns = getColumnsAmount(second);
  if (secondAmountOfColumns == 0)
  {
    unionResult = first;
    return;
  }

  ReplacementsHashes firstHashes;
  calculateHashesForCommonKeys(first, commonKeysSet, firstHashes);
  ReplacementsHashes secondHashes;
  calculateHashesForCommonKeys(second, commonKeysSet, secondHashes);
  std::unordered_set<size_t> columnsFromSecondToSkip;

  // insert into columnsFromSecondToSkip all column indices from second that are equal to at least one column from first
  for (auto const & pairForFirst : firstHashes)
  {
    std::vector<size_t> const & columnsFromFirst = pairForFirst.second;
    std::vector<size_t> const & columnsFromSecond =
        secondHashes.count(pairForFirst.first) ? secondHashes.at(pairForFirst.first) : std::vector<size_t>();
    for (auto const & columnFromSecond : columnsFromSecond)
    {
      bool isColumnFromSecondIdenticalToColumnFromFirst = true;
      for (auto const & columnFromFirst : columnsFromFirst)
      {
        for (auto const & commonKey : commonKeysSet)
        {
          if (first.at(commonKey).at(columnFromFirst) != second.at(commonKey).at(columnFromSecond))
            isColumnFromSecondIdenticalToColumnFromFirst = false;
          break;
        }
        if (!isColumnFromSecondIdenticalToColumnFromFirst)
          break;
      }
      if (isColumnFromSecondIdenticalToColumnFromFirst)
        columnsFromSecondToSkip.insert(columnFromSecond);
    }
  }

  Replacements result;
  // make all possible combinations for each column from first with each column from second with common keys values
  // taken from first
  for (auto const & pairForFirst : firstHashes)
  {
    std::vector<size_t> const & columnsFromFirst = pairForFirst.second;
    for (auto const & columnFromFirst : columnsFromFirst)
    {
      for (auto const & firstKey : firstKeys)
      {
        std::vector<ScAddr> const sameValuesToInsert(
            firstKeys.size() > commonKeysSet.size() ? secondAmountOfColumns : 1,
            first.at(firstKey).at(columnFromFirst));
        result[firstKey].insert(result.at(firstKey).cend(), sameValuesToInsert.cbegin(), sameValuesToInsert.cend());
      }
      for (auto const & secondKey : secondKeys)
      {
        if (!firstKeys.count(secondKey))
          result[secondKey].insert(
              result[secondKey].cend(), second.at(secondKey).cbegin(), second.at(secondKey).cend());
      }
    }
  }

  // make all possible combinations for each column from second with each column from first with common keys values
  // taken from second, skipping those columns from second that have same values as at least one column from first for
  // each common key
  for (auto const & pairForSecond : secondHashes)
  {
    std::vector<size_t> const & columnsFromSecond = pairForSecond.second;
    for (auto const & columnFromSecond : columnsFromSecond)
    {
      if (columnsFromSecondToSkip.count(columnFromSecond))
        continue;
      for (auto const & secondKey : secondKeys)
      {
        std::vector<ScAddr> const sameValuesToInsert(
            secondKeys.size() > commonKeysSet.size() ? firstAmountOfColumns : 1,
            second.at(secondKey).at(columnFromSecond));
        result[secondKey].insert(result.at(secondKey).cend(), sameValuesToInsert.cbegin(), sameValuesToInsert.cend());
      }
      for (auto const & firstKey : firstKeys)
      {
        if (!secondKeys.count(firstKey))
          result[firstKey].insert(result[firstKey].cend(), first.at(firstKey).cbegin(), first.at(firstKey).cend());
      }
    }
  }
  removeDuplicateColumns(result);
  unionResult = result;
}

void ReplacementsUtils::getKeySet(Replacements const & map, ScAddrUnorderedSet & keySet)
{
  for (auto const & pair : map)
    keySet.insert(pair.first);
}

void ReplacementsUtils::getCommonKeys(
    ScAddrUnorderedSet const & first,
    ScAddrUnorderedSet const & second,
    ScAddrUnorderedSet & commonKeys)
{
  ScAddrUnorderedSet result;
  for (ScAddr const & key : first)
  {
    if (second.find(key) != second.end())
      commonKeys.insert(key);
  }
}

Replacements ReplacementsUtils::copyReplacements(Replacements const & replacements)
{
  Replacements result;
  for (auto const & pair : replacements)
  {
    ScAddr const & key = pair.first;
    for (ScAddr const & value : replacements.at(key))
      result[key].push_back(value);
  }
  return result;
}

/**
 * @brief The size of the all ScAddrVector of variables is the same (it is a matrix)
 * @param replacements to convert to vector<ScTemplateParams>
 * @param templateParams out param, converted replacements will be placed here
 */
void ReplacementsUtils::getReplacementsToScTemplateParams(
    Replacements const & replacements,
    std::vector<ScTemplateParams> & templateParams)
{
  ScAddrUnorderedSet keys;
  getKeySet(replacements, keys);
  if (keys.empty())
    return;

  size_t columnsAmount = replacements.begin()->second.size();
  for (size_t columnIndex = 0; columnIndex < columnsAmount; ++columnIndex)
  {
    ScTemplateParams params;
    for (ScAddr const & key : keys)
      params.Add(key, replacements.at(key).at(columnIndex));
    templateParams.push_back(params);
  }
}

size_t ReplacementsUtils::getColumnsAmount(Replacements const & replacements)
{
  return (replacements.empty() ? 0 : replacements.begin()->second.size());
}

void ReplacementsUtils::removeDuplicateColumns(Replacements & replacements)
{
  ScAddrUnorderedSet keys;
  getKeySet(replacements, keys);
  if (keys.empty())
    return;
  ReplacementsHashes replacementsHashes;
  calculateHashesForCommonKeys(replacements, keys, replacementsHashes);
  std::unordered_map<ScAddr, ScAddr, ScAddrHashFunc> column;
  std::set<size_t> columnsToRemove;
  for (auto const & replacementsHash : replacementsHashes)
  {
    auto const & columnsForHash = replacementsHash.second;
    if (columnsForHash.size() > 1)
    {
      for (size_t firstColumnIndex = 0; firstColumnIndex < columnsForHash.size(); ++firstColumnIndex)
      {
        if (columnsToRemove.count(firstColumnIndex))
          continue;
        for (auto const & key : keys)
          column[key] = replacements.at(key).at(columnsForHash[firstColumnIndex]);
        for (size_t comparedColumnIndex = firstColumnIndex + 1; comparedColumnIndex < columnsForHash.size();
             ++comparedColumnIndex)
        {
          if (columnsToRemove.count(comparedColumnIndex))
            continue;
          bool columnIsUnique = false;
          for (auto const & key : keys)
          {
            if (column[key] != replacements.at(key).at(columnsForHash[comparedColumnIndex]))
            {
              columnIsUnique = true;
              break;
            }
          }
          if (!columnIsUnique)
            columnsToRemove.insert(columnsForHash[comparedColumnIndex]);
        }
      }
    }
  }
  for (auto columnToRemove = columnsToRemove.crbegin(); columnToRemove != columnsToRemove.crend(); columnToRemove++)
  {
    for (auto const & key : keys)
    {
      ScAddrVector & replacementValues = replacements.at(key);
      replacementValues.erase(replacementValues.begin() + static_cast<long>(*columnToRemove));
    }
  }
}

void ReplacementsUtils::calculateHashesForCommonKeys(
    Replacements const & replacements,
    ScAddrUnorderedSet const & commonKeys,
    ReplacementsHashes & hashes)
{
  size_t const columnsAmount = ReplacementsUtils::getColumnsAmount(replacements);
  size_t const commonKeysAmount = commonKeys.empty() ? 1 : commonKeys.size();
  std::vector<size_t> primes = {7, 13, 17, 19, 31, 41, 43};
  for (size_t columnNumber = 0; columnNumber < columnsAmount; ++columnNumber)
  {
    int primeInd = 0;
    size_t offsets = 0;
    for (auto const & commonKey : commonKeys)
      offsets +=
          replacements.at(commonKey).at(columnNumber).GetRealAddr().offset * primes.at(primeInd++ % primes.size());
    hashes[offsets / commonKeysAmount].push_back(columnNumber);
  }
}

Replacements ReplacementsUtils::removeRows(Replacements const & replacements, ScAddrUnorderedSet & keysToRemove)
{
  Replacements result;
  for (auto const & replacement : replacements)
  {
    if (keysToRemove.count(replacement.first))
      continue;
    result[replacement.first].assign(replacement.second.cbegin(), replacement.second.cend());
  }
  return result;
}
}  // namespace inference
