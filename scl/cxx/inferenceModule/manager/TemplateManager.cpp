/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <algorithm>
#include "TemplateManager.hpp"
#include <sc-agents-common/utils/IteratorUtils.hpp>

using namespace inference;

TemplateManager::TemplateManager(ScMemoryContext * ms_context)
{
  this->context = ms_context;
}

vector<ScTemplateParams> TemplateManager::createTemplateParamsList(
      const ScAddr & scTemplate,
      const vector<ScAddr> & argumentList)
{
  return createTemplateParams(scTemplate, argumentList);
}

vector<ScTemplateParams> TemplateManager::createTemplateParams(
      ScAddr const & scTemplate,
      const vector<ScAddr> & argumentList)
{
  map<string, set<ScAddr, AddrComparator>> replacementsMultimap;
  vector<ScTemplateParams> vectorOfTemplateParams;

  SC_LOG_DEBUG("***\ncreating template params\ntemplate name: " + context->HelperGetSystemIdtf(scTemplate))

  ScIterator3Ptr varIterator = context->Iterator3(
        scTemplate,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    string varName = context->HelperGetSystemIdtf(var);
    SC_LOG_DEBUG("found variable---> " + context->HelperGetSystemIdtf(var));
    ScAddr argumentOfVar;
    ScIterator5Ptr classesIterator = context->Iterator5(
          ScType::NodeConstClass,
          ScType::EdgeAccessVarPosPerm,
          var,
          ScType::EdgeAccessConstPosPerm,
          scTemplate);
    while (classesIterator->Next())
    {
      ScAddr varClass = classesIterator->Get(0);
      for (auto & argument : argumentList) // this block is executed if inputStructure is valid
      {
        if (context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm))
        {
          SC_LOG_DEBUG("adding " + context->HelperGetSystemIdtf(argument) + " into " + varName)
          replacementsMultimap[varName].insert(argument);
        }
      }
      if (argumentList.empty()) // this block is executed if inputStructure is not valid
      {
        ScIterator3Ptr iterator3 = context->Iterator3(varClass, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (iterator3->Next())
        {
          auto argument = iterator3->Get(2);
          SC_LOG_DEBUG("adding " + context->HelperGetSystemIdtf(argument) + " into " + varName)
          replacementsMultimap[varName].insert(argument);
        }
      }
    }
    if (vectorOfTemplateParams.empty())
    {
      auto addresses = replacementsMultimap[varName];
      vectorOfTemplateParams.reserve(replacementsMultimap[varName].size());
      for (auto const & address : addresses)
      {
        ScTemplateParams params;
        params.Add(varName, address);
        vectorOfTemplateParams.push_back(params);
      }
    }
    else
    {
      auto addresses = replacementsMultimap[varName];
      auto amountOfAddressesForVar = addresses.size();
      auto oldParamsSize = vectorOfTemplateParams.size();
      auto amountOfNewElements = oldParamsSize * (amountOfAddressesForVar - 1);
      vectorOfTemplateParams.reserve(amountOfNewElements);
      int beginOfCopy = 0;
      int endOfCopy = oldParamsSize;
      for (auto const & address : addresses)
      {
        copy_n(
              vectorOfTemplateParams.begin() + beginOfCopy,
               oldParamsSize,
               back_inserter(vectorOfTemplateParams));
        for (int i = 0; i < oldParamsSize; ++i)
        {
          SC_LOG_DEBUG("adding <" + varName + ", " + context->HelperGetSystemIdtf(address) + "> at index " + to_string(beginOfCopy + i))
          vectorOfTemplateParams[beginOfCopy + i].Add(varName, address);
        }
        beginOfCopy = endOfCopy;
        endOfCopy += oldParamsSize;
      }

    }

  }
  SC_LOG_DEBUG("***")

  SC_LOG_DEBUG("before exit size of vector with params is " + to_string(vectorOfTemplateParams.size()))
  return vectorOfTemplateParams;
}

void TemplateManager::addVarToReplacementsList(
      std::vector<std::map<ScAddr, string, AddrComparator>> & replacementsList,
      string & varName,
      std::vector<ScAddr> & argumentOfVarList)
{
  if (!argumentOfVarList.empty())
  {
    std::vector<std::map<ScAddr, string, AddrComparator>> newReplacementsList;

    if (!replacementsList.empty())
    {
      for (auto & replacementsMap : replacementsList)
      {
        for (auto & argumentOfVar : argumentOfVarList)
        {
          if (!replacementsMap.count(argumentOfVar))
          {
            std::map<ScAddr, string, AddrComparator> newReplacementsMap = replacementsMap;
            newReplacementsMap.insert(std::make_pair(argumentOfVar, varName));
            newReplacementsList.push_back(newReplacementsMap);
          }
        }
      }
    }
    else
    {
      for (auto & argumentOfVar : argumentOfVarList)
      {
        std::map<ScAddr, string, AddrComparator> newReplacementsMap;
        newReplacementsMap.insert(std::make_pair(argumentOfVar, varName));
        newReplacementsList.push_back(newReplacementsMap);
      }
    }

    replacementsList = newReplacementsList;
  }
}

vector<ScTemplateParams> TemplateManager::createTemplateParamsList(
      vector<map<ScAddr, string, AddrComparator>> & replacementsList)
{
  SC_LOG_DEBUG("Creating template params for rule usage");
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
    SC_LOG_DEBUG("***");
  }
  SC_LOG_DEBUG("Created template params for rule usage");
  return templateParamsList;
}
