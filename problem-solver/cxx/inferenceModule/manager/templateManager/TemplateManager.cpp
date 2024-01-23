/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateManager.hpp"

#include <algorithm>

using namespace inference;

TemplateManager::TemplateManager(ScMemoryContext * ms_context)
  : TemplateManagerAbstract(ms_context)
{
}

/**
 * For all classes of the all template variables create map <varName, arguments>
 * Where arguments are elements from argumentList, and each argument class is the same as variable varName class
 */
std::vector<ScTemplateParams> TemplateManager::createTemplateParams(ScAddr const & scTemplate)
{
  std::map<ScAddr, std::set<ScAddr, ScAddrLessFunc>, ScAddrLessFunc> replacementsMultimap;
  std::vector<ScTemplateParams> templateParamsVector;

  ScIterator3Ptr variableNodeIterator = context->Iterator3(scTemplate, ScType::EdgeAccessConstPosPerm, ScType::NodeVar);
  while (variableNodeIterator->Next())
  {
    ScAddr const & variableNode = variableNodeIterator->Get(2);
    if (!replacementsMultimap[variableNode].empty())
    {
      continue;
    }
    ScIterator5Ptr constantsIterator = context->Iterator5(
        ScType::NodeConstClass, ScType::EdgeAccessVarPosPerm, variableNode, ScType::EdgeAccessConstPosPerm, scTemplate);
    while (constantsIterator->Next())
    {
      ScAddr const & varClass = constantsIterator->Get(0);
      for (ScAddr const & argument : arguments)
      {
        if (context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm))
          replacementsMultimap[variableNode].insert(argument);
      }
    }
    if (templateParamsVector.empty())
    {
      std::set<ScAddr, ScAddrLessFunc> addresses = replacementsMultimap[variableNode];
      templateParamsVector.reserve(replacementsMultimap[variableNode].size());
      for (ScAddr const & address : addresses)
      {
        ScTemplateParams params;
        params.Add(variableNode, address);
        templateParamsVector.push_back(params);
      }
    }
    else
    {
      std::set<ScAddr, ScAddrLessFunc> addresses = replacementsMultimap[variableNode];
      size_t amountOfAddressesForVar = addresses.size();
      size_t oldParamsSize = templateParamsVector.size();

      if (amountOfAddressesForVar == 0)
        continue;

      size_t amountOfNewElements = oldParamsSize * amountOfAddressesForVar;
      std::vector<ScTemplateParams> updatedParams;
      updatedParams.reserve(amountOfNewElements);
      size_t beginOfCopy = 0;
      size_t endOfCopy = oldParamsSize;
      for (ScAddr const & address : addresses)
      {
        copy_n(templateParamsVector.begin(), oldParamsSize, back_inserter(updatedParams));
        for (size_t i = 0; i < oldParamsSize; ++i)
        {
          updatedParams[beginOfCopy + i].Add(variableNode, address);
        }

        beginOfCopy = endOfCopy;
        endOfCopy += oldParamsSize;
      }

      templateParamsVector = std::move(updatedParams);
    }
  }
  return templateParamsVector;
}
