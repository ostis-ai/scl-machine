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
    ScAddr const & scTemplate,
    vector<ScAddr> const & argumentList)
{
  return createTemplateParams(scTemplate, argumentList);
}

vector<ScTemplateParams> TemplateManager::createTemplateParams(
      ScAddr const & scTemplate,
      const vector<ScAddr> & argumentList)
{
  map<string, set<ScAddr, AddrComparator>> replacementsMultimap;
  vector<ScTemplateParams> vectorOfTemplateParams;

  SC_LOG_DEBUG("***\ncreating template params\ntemplate name: " + context->HelperGetSystemIdtf(scTemplate));

  ScIterator3Ptr varIterator = context->Iterator3(scTemplate, ScType::EdgeAccessConstPosPerm, ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    string varName = context->HelperGetSystemIdtf(var);
    if (!replacementsMultimap[varName].empty())
    {
      SC_LOG_DEBUG("var " + varName + " has been processed already");
      continue;
    }
    ScAddr argumentOfVar;
    ScIterator5Ptr classesIterator = context->Iterator5(
        ScType::NodeConstClass, ScType::EdgeAccessVarPosPerm, var, ScType::EdgeAccessConstPosPerm, scTemplate);
    while (classesIterator->Next())
    {
      ScAddr varClass = classesIterator->Get(0);
      for (auto & argument : argumentList) // this block is executed if inputStructure is valid
      {
        if (context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm))
          replacementsMultimap[varName].insert(argument);
      }
      if (argumentList.empty()) // this block is executed if inputStructure is not valid
      {
        ScIterator3Ptr iterator3 = context->Iterator3(varClass, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (iterator3->Next())
          replacementsMultimap[varName].insert(iterator3->Get(2));
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
          vectorOfTemplateParams[beginOfCopy + i].Add(varName, address);
        beginOfCopy = endOfCopy;
        endOfCopy += oldParamsSize;
      }
    }
  }
  SC_LOG_DEBUG("***");
  return vectorOfTemplateParams;
}
