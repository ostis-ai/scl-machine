/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherAbstract.hpp"

#include <algorithm>

#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

using namespace inference;

TemplateSearcherAbstract::TemplateSearcherAbstract(ScMemoryContext * context) : context(context) { }

void TemplateSearcherAbstract::setInputStructures(ScAddr const & otherInputStructures)
{
  inputStructures = otherInputStructures;
}

Replacements TemplateSearcherAbstract::searchTemplate(
      ScAddr const & templateAddr,
      vector<ScTemplateParams> const & scTemplateParamsVector)
{
  Replacements result;
  ScAddr argument;
  std::set<string> varNames = getVarNames(templateAddr);
  for (ScTemplateParams const & scTemplateParams : scTemplateParamsVector)
  {
    vector<ScTemplateSearchResultItem> searchResults = searchTemplate(templateAddr, scTemplateParams);
    for (ScTemplateSearchResultItem const & searchResult : searchResults)
    {
      for (std::string const & varName : varNames)
      {
        if (scTemplateParams.Get(varName, argument))
          result[varName].push_back(argument);
        else if (searchResult.Has(varName))
          result[varName].push_back(searchResult[varName]);
      }
    }
  }

  return result;
}

Replacements TemplateSearcherAbstract::searchTemplate(ScAddr const & templateAddr)
{
  Replacements result;
  std::set<std::string> varNames = getVarNames(templateAddr);
  ScTemplateParams blankParams;
  vector<ScTemplateSearchResultItem> searchResults = searchTemplate(templateAddr, blankParams);
  for (ScTemplateSearchResultItem const & searchResult : searchResults)
  {
    for (std::string const & varName : varNames)
      result[varName].push_back(searchResult[varName]);
  }

  return result;
}

std::set<std::string> TemplateSearcherAbstract::getVarNames(ScAddr const & structure)
{
  std::set<std::string> identifiers;
  auto collectVarIdtfs = [this, structure](ScType const & varType, std::set<std::string> & identifiers) {
    ScIterator3Ptr variablesIter3 = context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, varType);
    while (variablesIter3->Next())
    {
      std::string const variableSystemIdtf = context->HelperGetSystemIdtf(variablesIter3->Get(2));
      if (!variableSystemIdtf.empty())
        identifiers.insert(variableSystemIdtf);
    }
  };
  collectVarIdtfs(ScType::NodeVar, identifiers);
  collectVarIdtfs(ScType::LinkVar, identifiers);
  return identifiers;
}

const ScAddrVector & TemplateSearcherAbstract::getParams() const
{
  return params;
}

void TemplateSearcherAbstract::addParam(ScAddr const & param)
{
  params.push_back(param);
}

bool TemplateSearcherAbstract::addParamIfNotPresent(ScAddr const & param)
{
  if (std::find(params.begin(), params.end(), param) == std::end(params))
  {
    params.push_back(param);
    if (arguments.IsValid())
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, arguments, param);
    return true;
  }
  return false;
}

void TemplateSearcherAbstract::setArguments(ScAddr const & otherArguments)
{
  this->arguments = otherArguments;
}
