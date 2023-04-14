/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateManager.hpp"

#include <algorithm>

using namespace inference;

TemplateManager::TemplateManager(ScMemoryContext * ms_context)
{
  context = ms_context;
}

/* For all classes of the all template variables create map <varName, arguments>
 * Where arguments are elements from argumentList, and each argument class is the same as variable varName class
 */
std::vector<ScTemplateParams> TemplateManager::createTemplateParams(
    ScAddr const & scTemplate,
    ScAddrVector const & argumentList)
{
  std::unordered_map<std::string, std::set<ScAddr, AddrComparator>> replacementsMultimap;

  ScIterator3Ptr const & varIterator = context->Iterator3(scTemplate, ScType::EdgeAccessConstPosPerm, ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    std::string varName = context->HelperGetSystemIdtf(var);
    ScAddr argumentOfVar;
    for (ScAddr const & argument : argumentList)
    {
      ScIterator5Ptr const & classesIterator = context->Iterator5(
          ScType::NodeConstClass, ScType::EdgeAccessVarPosPerm, var, ScType::EdgeAccessConstPosPerm, scTemplate);
      bool isArgumentBelongToVarClasses = false;
      while (classesIterator->Next())
      {
        ScAddr varClass = classesIterator->Get(0);
        if (context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm))
        {
          isArgumentBelongToVarClasses = true;
        }
        else
        {
          isArgumentBelongToVarClasses = false;
          break;
        }
      }

      if (isArgumentBelongToVarClasses)
      {
        replacementsMultimap[varName].insert(argument);
      }
    }
  }

  std::vector<std::unordered_map<std::string, ScAddr>> paramsVector;
  paramsVector.reserve(replacementsMultimap.size());
  for (auto const & pair : replacementsMultimap)
  {
    std::vector<std::unordered_map<std::string, ScAddr>> newParamsVector;
    newParamsVector.reserve(paramsVector.size() * pair.second.size());
    for (auto const & addr : pair.second)
    {
      bool isUploaded = false;
      for (auto map : paramsVector)
      {
        bool isDuplicatedValue = false;
        for (auto const & mapPair : map)
        {
          if (mapPair.second == addr)
          {
            isDuplicatedValue = true;
          }
        }

        if (!isDuplicatedValue)
        {
          map.insert({ pair.first, addr });
          newParamsVector.emplace_back(map);
        }

        isUploaded = true;
      }

      if (!isUploaded)
      {
        std::unordered_map<std::string, ScAddr> map{{ pair.first, addr }};
        newParamsVector.emplace_back(map);
      }
    }
    paramsVector = newParamsVector;
  }

  std::vector<ScTemplateParams> templateParamsVector;
  for (auto const & params : paramsVector)
  {
    ScTemplateParams templateParams;
    for (auto const & param : params)
    {
      templateParams.Add(param.first, param.second);
    }
    templateParamsVector.emplace_back(templateParams);
  }

  return templateParamsVector;
}
