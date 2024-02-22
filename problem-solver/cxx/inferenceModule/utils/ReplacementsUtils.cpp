/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ReplacementsUtils.hpp"
#include "sc-memory/kpm/sc_agent.hpp"

namespace inference
{

Replacements ReplacementsUtils::intersectReplacements(
    Replacements const & first,
    Replacements const & second)
{
  Replacements result;
  std::vector<std::pair<size_t, size_t>> firstSecondPairs;
  ScAddrHashSet firstKeys;
  getKeySet(first, firstKeys);
  ScAddrHashSet secondKeys;
  getKeySet(second, secondKeys);
  size_t firstAmountOfColumns = getColumnsAmount(first);
  size_t secondAmountOfColumns = getColumnsAmount(second);

  if (firstAmountOfColumns == 0)
    return copyReplacements(second);
  if (secondAmountOfColumns == 0)
    return copyReplacements(first);

  ScAddrHashSet commonKeysSet = getCommonKeys(firstKeys, secondKeys);

  ReplacementsHashes firstHashes = calculateHashesForCommonKeys(first, commonKeysSet);
  ReplacementsHashes secondHashes = calculateHashesForCommonKeys(second, commonKeysSet);
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
  return result;
}

Replacements ReplacementsUtils::subtractReplacements(Replacements const & first, Replacements const & second)
{
  Replacements result;
  ScAddrHashSet firstKeys;
  getKeySet(first, firstKeys);
  ScAddrHashSet secondKeys;
  getKeySet(second, secondKeys);
  size_t firstAmountOfColumns = getColumnsAmount(first);
  size_t secondAmountOfColumns = getColumnsAmount(second);
  std::vector<size_t> firstColumns;
  firstColumns.reserve(firstAmountOfColumns);

  if (firstAmountOfColumns == 0 || secondAmountOfColumns == 0)
    return copyReplacements(first);

  ScAddrHashSet commonKeysSet = getCommonKeys(firstKeys, secondKeys);

  if (commonKeysSet.empty())
    return copyReplacements(first);

  ReplacementsHashes firstHashes = calculateHashesForCommonKeys(first, commonKeysSet);
  ReplacementsHashes secondHashes = calculateHashesForCommonKeys(second, commonKeysSet);
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

  for (ScAddr const & firstKey : firstKeys)
    result[firstKey].reserve(firstColumns.size());

  for (auto const & firstColumn : firstColumns)
  {
    for (ScAddr const & firstKey : firstKeys)
      result.at(firstKey).push_back(first.at(firstKey).at(firstColumn));
  }
  removeDuplicateColumns(result);
  return result;
}

Replacements ReplacementsUtils::uniteReplacements(Replacements const & first, Replacements const & second)
{
  Replacements result;
  ScAddrHashSet firstKeys;
  getKeySet(first, firstKeys);
  ScAddrHashSet secondKeys;
  getKeySet(second, secondKeys);
  ScAddrHashSet commonKeysSet = getCommonKeys(firstKeys, secondKeys);
  size_t firstAmountOfColumns = getColumnsAmount(first);
  if (firstAmountOfColumns == 0)
    return copyReplacements(second);
  size_t secondAmountOfColumns = getColumnsAmount(second);
  if (secondAmountOfColumns == 0)
    return copyReplacements(first);

  ReplacementsHashes const & firstHashes = calculateHashesForCommonKeys(first, commonKeysSet);
  ReplacementsHashes const & secondHashes = calculateHashesForCommonKeys(second, commonKeysSet);
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
        result[firstKey].insert(result[firstKey].cend(), sameValuesToInsert.cbegin(), sameValuesToInsert.cend());
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
        result[secondKey].insert(result[secondKey].cend(), sameValuesToInsert.cbegin(), sameValuesToInsert.cend());
      }
      for (auto const & firstKey : firstKeys)
      {
        if (!secondKeys.count(firstKey))
          result[firstKey].insert(result[firstKey].cend(), first.at(firstKey).cbegin(), first.at(firstKey).cend());
      }
    }
  }
  removeDuplicateColumns(result);
  return result;
}

void ReplacementsUtils::getKeySet(Replacements const & map, ScAddrHashSet & keySet)
{
  for (auto const & pair : map)
    keySet.insert(pair.first);
}

ScAddrHashSet ReplacementsUtils::getCommonKeys(ScAddrHashSet const & first, ScAddrHashSet const & second)
{
  ScAddrHashSet result;
  for (ScAddr const & key : first)
  {
    if (second.find(key) != second.end())
      result.insert(key);
  }
  return result;
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
 * @return vector<ScTemplateParams> of converted replacements
 */
vector<ScTemplateParams> ReplacementsUtils::getReplacementsToScTemplateParams(
    Replacements const & replacements)
{
  vector<ScTemplateParams> result;
  ScAddrHashSet keys;
  getKeySet(replacements, keys);
  if (keys.empty())
    return result;

  size_t columnsAmount = replacements.begin()->second.size();
  for (size_t columnIndex = 0; columnIndex < columnsAmount; ++columnIndex)
  {
    ScTemplateParams params;
    for (ScAddr const & key : keys)
      params.Add(key, replacements.at(key).at(columnIndex));
    result.push_back(params);
  }
  return result;
}

size_t ReplacementsUtils::getColumnsAmount(Replacements const & replacements)
{
  return (replacements.empty() ? 0 : replacements.begin()->second.size());
}

void ReplacementsUtils::removeDuplicateColumns(Replacements & replacements)
{
  ScAddrHashSet keys;
  getKeySet(replacements, keys);
  if (keys.empty())
    return;
  ReplacementsHashes const & replacementsHashes = calculateHashesForCommonKeys(replacements, keys);
  std::unordered_map<ScAddr, ScAddr, ScAddrHashFunc<uint32_t>> column;
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

ReplacementsHashes ReplacementsUtils::calculateHashesForCommonKeys(
    Replacements const & replacements,
    ScAddrHashSet const & commonKeys)
{
  ReplacementsHashes replacementsHashes;
  size_t const columnsAmount = ReplacementsUtils::getColumnsAmount(replacements);
  size_t const commonKeysAmount = commonKeys.empty() ? 1 : commonKeys.size();
  std::vector<size_t> primes = {7, 13, 17, 19, 31, 41, 43};
  for (size_t columnNumber = 0; columnNumber < columnsAmount; ++columnNumber)
  {
    int primeInd = 0;
    size_t offsets = 0;
    for (auto const & commonKey : commonKeys)
      offsets += replacements.at(commonKey).at(columnNumber).GetRealAddr().offset *
                 primes.at(primeInd++ % primes.size());
    replacementsHashes[offsets / commonKeysAmount].push_back(columnNumber);
  }
  return replacementsHashes;
}

Replacements ReplacementsUtils::removeRows(Replacements const & replacements, ScAddrHashSet & keysToRemove)
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
