/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateManager.hpp"

#include <algorithm>

using namespace inference;

TemplateManager::TemplateManager(ScMemoryContext * ms_context) : TemplateManagerAbstract(ms_context)
{
}

/* For all classes of the all template variables create map <varName, arguments>
 * Where arguments are elements from argumentList, and each argument class is the same as variable varName class
 */
std::vector<ScTemplateParams> TemplateManager::createTemplateParams(
    ScAddr const & scTemplate,
    ScAddrVector const & argumentList)
{
  std::map<std::string, std::set<ScAddr, AddrComparator>> replacementsMultimap;
  std::vector<ScTemplateParams> templateParamsVector;

  ScIterator3Ptr varIterator = context->Iterator3(scTemplate, ScType::EdgeAccessConstPosPerm, ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    std::string varName = context->HelperGetSystemIdtf(var);
    if (!replacementsMultimap[varName].empty())
    {
      continue;
    }
    ScAddr argumentOfVar;
    ScIterator5Ptr classesIterator = context->Iterator5(
          // TODO(MksmOrlov): why not ScType::NodeConst ?
        ScType::NodeConstClass, ScType::EdgeAccessVarPosPerm, var, ScType::EdgeAccessConstPosPerm, scTemplate);
    // TODO(MksmOrlov): Почему итерируемся по классам и для них ищем есть ли аргумент, а не от аргумента ищем, есть ли соответствующий класс
    while (classesIterator->Next())
    {
      ScAddr varClass = classesIterator->Get(0);
      // TODO(MksmOrlov): make cycle for argumentList main. If it is empty, a lot of useless operations are done
      for (ScAddr const & argument : argumentList)
      {
        if (context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm))
          replacementsMultimap[varName].insert(argument);
      }
    }
    if (templateParamsVector.empty())
    {
      std::set<ScAddr, AddrComparator> addresses = replacementsMultimap[varName];
      templateParamsVector.reserve(replacementsMultimap[varName].size());
      for (ScAddr const & address : addresses)
      {
        ScTemplateParams params;
        params.Add(varName, address);
        templateParamsVector.push_back(params);
      }
    }
    else
    {
      std::set<ScAddr, AddrComparator> addresses = replacementsMultimap[varName];
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
          updatedParams[beginOfCopy + i].Add(varName, address);
        }

        beginOfCopy = endOfCopy;
        endOfCopy += oldParamsSize;
      }

      templateParamsVector = std::move(updatedParams);
    }
  }
  return templateParamsVector;
}
