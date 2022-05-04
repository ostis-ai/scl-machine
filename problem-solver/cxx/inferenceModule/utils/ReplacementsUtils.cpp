//
// Created by vital on 03.05.2022.
//

#include "ReplacementsUtils.hpp"
#include "sc-memory/kpm/sc_agent.hpp"

map<string, vector<ScAddr>>
inference::ReplacementsUtils::intersectionOfReplacements(const map<string, vector<ScAddr>> & first,
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
  auto firstAmountOfColumns = (firstKeysSize == 0 || first.begin()->second.empty() ? 0 : first.begin()->second.size());
  auto secondAmountOfColumns = (secondKeysSize == 0 || second.begin()->second.empty() ? 0 : second.begin()->second.size());
  SC_LOG_DEBUG("columns: " + to_string(firstAmountOfColumns) + ", " + to_string(secondAmountOfColumns))
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

map<string, vector<ScAddr>> inference::ReplacementsUtils::unionOfReplacements(const map<string, vector<ScAddr>> & first,
                                                                              const map<string, vector<ScAddr>> & second)
{
  map<string, vector<ScAddr>> result;
  int resultSize = 0;
  auto firstKeys = keySet(first);
  auto secondKeys = keySet(second);
  auto commonKeysSet = commonKeys(firstKeys, secondKeys);
  auto firstKeysSize = firstKeys.size();
  auto secondKeysSize = secondKeys.size();
  auto firstAmountOfColumns = (firstKeysSize == 0 || first.begin()->second.empty() ? 0 : first.begin()->second.size());
  auto secondAmountOfColumns = (secondKeysSize == 0 || second.begin()->second.empty() ? 0 : second.begin()->second.size());
  if (!firstKeysSize || !firstAmountOfColumns)
    return copyReplacements(second);
  if (!secondKeysSize || !secondAmountOfColumns)
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

/*

 vector<ScTemplateParams> TemplateManager::createTemplateParamsList(
      vector<map<ScAddr, string, AddrComparator>> & replacementsList)
{
  SC_LOG_DEBUG("Creating template params for rule usage")
  std::vector<ScTemplateParams> templateParamsList;
  for (auto & replacementsMap : replacementsList)
  {
    ScTemplateParams scTemplateParams;
    for (std::pair<ScAddr, string> replacement : replacementsMap)
    {
      SC_LOG_DEBUG(replacement.second + " is " + std::to_string(replacement.first.GetRealAddr().seg) + "/" +
                   std::to_string(replacement.first.GetRealAddr().offset));
      scTemplateParams.Add(replacement.second, replacement.first);
    }
    templateParamsList.push_back(scTemplateParams);
    SC_LOG_DEBUG("***")
  }
  SC_LOG_DEBUG("Created template params for rule usage")
  return templateParamsList;
}
 */