//
// Created by vital on 03.05.2022.
//

#include "ReplacementsUtils.hpp"
#include "sc-memory/kpm/sc_agent.hpp"

map<string, vector<ScAddr>>
inference::ReplacementsUtils::intersectionOfReplacements(
      const map<string, vector<ScAddr>> & first,
      const map<string, vector<ScAddr>> & second)
{
  map<string, vector<ScAddr>> result;
  int resultSize = 0;
  auto firstKeys = keySet(first);
  auto secondKeys = keySet(second);
  auto commonKeysSet = commonKeys(firstKeys, secondKeys);
  auto firstKeysSize = firstKeys.size();
  auto secondKeysSize = secondKeys.size();
  SC_LOG_DEBUG("keys: " + to_string(firstKeysSize) + ", " + to_string(secondKeysSize))
  auto firstAmountOfColumns = getColumnsAmount(first);
  auto secondAmountOfColumns = getColumnsAmount(second);
  SC_LOG_DEBUG("columns: " + to_string(firstAmountOfColumns) + ", " + to_string(secondAmountOfColumns))

  if (firstAmountOfColumns == 0)
  {
    return copyReplacements(second);
  }
  if (secondAmountOfColumns == 0)
  {
    return copyReplacements(first);
  }

  for (size_t columnIndexInFirst = 0; columnIndexInFirst < firstAmountOfColumns; ++columnIndexInFirst)
  {
    for (size_t columnIndexInSecond = 0; columnIndexInSecond < secondAmountOfColumns; ++columnIndexInSecond)
    {
      bool commonPartsAreIdentical = true;
      SC_LOG_DEBUG("------------\n")
      for (auto const & commonKey : commonKeysSet)
      {
        SC_LOG_DEBUG("Comparing " + to_string(first.find(commonKey)->second[columnIndexInFirst].Hash()) + " and " + to_string(second.find(commonKey)->second[columnIndexInSecond].Hash()))
        if (first.find(commonKey)->second[columnIndexInFirst] != second.find(commonKey)->second[columnIndexInSecond])
          commonPartsAreIdentical = false;
      }
      if (commonPartsAreIdentical)
      {
        SC_LOG_DEBUG("identical for columns " + to_string(columnIndexInFirst) + " and " + to_string(columnIndexInSecond))
        for (auto const & firstKey : firstKeys)
          result[firstKey].push_back(first.find(firstKey)->second[columnIndexInFirst]);
        for (auto const & secondKey : secondKeys)
        {
          if (result[secondKey].size() == resultSize)
            result[secondKey].push_back(second.find(secondKey)->second[columnIndexInSecond]);
        }
        ++resultSize;
      }
    }
  }
  return result;
}

map<string, vector<ScAddr>> inference::ReplacementsUtils::unionOfReplacements(
      const map<string, vector<ScAddr>> & first,
      const map<string, vector<ScAddr>> & second)
{
  map<string, vector<ScAddr>> result;
  int resultSize = 0;
  auto firstKeys = keySet(first);
  auto secondKeys = keySet(second);
  auto commonKeysSet = commonKeys(firstKeys, secondKeys);
  auto firstKeysSize = firstKeys.size();
  auto secondKeysSize = secondKeys.size();
  auto firstAmountOfColumns = getColumnsAmount(first);
  auto secondAmountOfColumns = getColumnsAmount(second);

  if (firstAmountOfColumns == 0)
    return copyReplacements(second);
  if (secondAmountOfColumns == 0)
    return copyReplacements(first);

  for (size_t columnIndexInFirst = 0; columnIndexInFirst < firstAmountOfColumns; ++columnIndexInFirst)
  {
    for (size_t columnIndexInSecond = 0; columnIndexInSecond < secondAmountOfColumns; ++columnIndexInSecond)
    {
      for (auto const & firstKey : firstKeys)
        result[firstKey].push_back(first.find(firstKey)->second[columnIndexInFirst]);
      for (auto const & secondKey : secondKeys)
      {
        if (result[secondKey].size() == resultSize)
          result[secondKey].push_back(second.find(secondKey)->second[columnIndexInSecond]);
      }
      ++resultSize;
      for (auto const & secondKey : secondKeys)
        result[secondKey].push_back(second.find(secondKey)->second[columnIndexInSecond]);
      for (auto const & firstKey : firstKeys)
      {
        if (result[firstKey].size() == resultSize)
          result[firstKey].push_back(first.find(firstKey)->second[columnIndexInFirst]);
      }
      ++resultSize;
    }
  }
  return result;
}

set<string> inference::ReplacementsUtils::keySet(const map<string, vector<ScAddr>> & map)
{
  set<string> keySet;
  for (auto const & pair : map)
    keySet.insert(pair.first);
  return keySet;
}

set<string> inference::ReplacementsUtils::commonKeys(const set<string> & first, const set<string> & second)
{
  set<string> result;
  for (auto const & key : first)
  {
    if (second.find(key) != second.end())
      result.insert(key);
  }
  return result;
}

map<string, vector<ScAddr>> inference::ReplacementsUtils::copyReplacements(map<string, vector<ScAddr>> const & replacements)
{
  map<string, vector<ScAddr>> result;
  for (auto const & pair : replacements)
  {
    auto const & key = pair.first;
    for (auto const & value : replacements.find(key)->second)
      result[key].push_back(value);
  }
  return result;
}

vector<ScTemplateParams>
inference::ReplacementsUtils::replacementsToScTemplateParams(const map<string, vector<ScAddr>> & replacements)
{
  vector<ScTemplateParams> result;
  auto keys = keySet(replacements);
  if (keys.empty())
    return result;

  auto columnsAmount = replacements.begin()->second.size();
  for (int columnIndex = 0; columnIndex < columnsAmount; ++columnIndex)
  {
    ScTemplateParams params;
    for (auto const & key : keys)
      params.Add(key, replacements.find(key)->second[columnIndex]);
    result.push_back(params);
  }
  return result;
}

size_t inference::ReplacementsUtils::getColumnsAmount(map<string, vector<ScAddr>> const & replacements)
{
  return (replacements.empty() ? 0 : replacements.begin()->second.size());
}

size_t inference::ReplacementsUtils::getRowsAmount(map<string, vector<ScAddr>> const & replacements)
{
  return replacements.size();
}

