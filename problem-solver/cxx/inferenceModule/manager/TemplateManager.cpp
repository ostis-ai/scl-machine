/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "TemplateManager.hpp"

using namespace inference;

TemplateManager::TemplateManager(ScMemoryContext * ms_context)
{
  this->context = ms_context;
}

vector<ScTemplateParams> TemplateManager::createTemplateParamsList(
      const ScAddr & scTemplate,
      const vector<ScAddr> & argumentList)
{
  std::vector<std::map<ScAddr, string, AddrComparator>> replacementsList;
  ScIterator3Ptr varIterator = context->Iterator3(
        scTemplate,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeVar);
  //Program can crash here
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    string varName = context->HelperGetSystemIdtf(var);
    std::vector<ScAddr> argumentOfVarList;
    ScIterator5Ptr classesIterator = context->Iterator5(
          ScType::NodeConstClass,
          ScType::EdgeAccessVarPosPerm,
          var,
          ScType::EdgeAccessConstPosPerm,
          scTemplate);
    while (classesIterator->Next())
    {
      ScAddr varClass = classesIterator->Get(0);
      for (auto & argument : argumentList)
      {
        if ((context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm)) && (argument.IsValid()))
        {
          argumentOfVarList.push_back(argument);
        }
      }
    }
    if (!argumentList.empty())
    {
      addVarToReplacementsList(replacementsList, varName, argumentOfVarList);
    }
  }

  return createTemplateParamsList(replacementsList);
}

ScTemplateParams TemplateManager::createTemplateParams(
      ScAddr const & scTemplate,
      const vector<ScAddr> & argumentList)
{
  map<ScAddr, string, AddrComparator> replacementsMap;
  SC_LOG_DEBUG("***\ncreating template params\ntemplate name: " + context->HelperGetSystemIdtf(scTemplate));

  ScIterator3Ptr varIterator = context->Iterator3(
        scTemplate,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    SC_LOG_DEBUG("found variable: " + context->HelperGetSystemIdtf(var));
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
      for (auto & argument : argumentList)
      {
        if (context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm))
        {
          if (!replacementsMap.count(argument))
          {
            argumentOfVar = argument;
            break;
          }
        }
      }
      if (argumentOfVar.IsValid())
      {
        string varName = context->HelperGetSystemIdtf(var);
        replacementsMap.insert(make_pair(argumentOfVar, varName));
        SC_LOG_DEBUG("variable class: " + context->HelperGetSystemIdtf(varClass));
        SC_LOG_DEBUG(varName + " is " + to_string(argumentOfVar.GetRealAddr().seg) + "/" +
                     to_string(argumentOfVar.GetRealAddr().offset));
        break;
      }
    }
  }
  SC_LOG_DEBUG("***");

  ScTemplateParams templateParams;
  for (const auto & replacement : replacementsMap)
  {
    templateParams.Add(replacement.second, replacement.first);
  }
  return templateParams;
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
