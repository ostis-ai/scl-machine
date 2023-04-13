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

TemplateSearcherAbstract::TemplateSearcherAbstract(ScMemoryContext * context) : context(context)
{
}

void TemplateSearcherAbstract::setInputStructures(ScAddr const & otherInputStructures)
{
  inputStructures = otherInputStructures;
}

Replacements TemplateSearcherAbstract::searchTemplate(
      ScAddr const & templateAddr,
      vector<ScTemplateParams> const & scTemplateParamsVector)
{
  Replacements result;
  Replacements searchResults;
  ScAddrVector varNameReplacementValues;
  ScAddr argument;
  std::set<string> const & varNames = getVarNames(templateAddr);

  for (ScTemplateParams const & scTemplateParams : scTemplateParamsVector)
  {
    searchResults = searchTemplate(templateAddr, scTemplateParams);
    for (std::string const & varName : varNames)
    {
      if (searchResults.count(varName))
      {
        varNameReplacementValues = searchResults.at(varName);
        if (scTemplateParams.Get(varName, argument))
        {
          result[varName].push_back(argument);
        }
        else if (!varNameReplacementValues.empty())
        {
          result[varName] = varNameReplacementValues;
        }
      }
    }
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

bool TemplateSearcherAbstract::isContentIdentical(ScTemplateSearchResultItem const & item, std::map<std::string, std::string> const & linksContentMap)
{
  bool result = true;
  ScAddr link;
  std::string linkContent;
  for (auto const & contentMap : linksContentMap)
  {
    item.Get(contentMap.first, link);
    context->GetLinkContent(link, linkContent);
    if (contentMap.second != linkContent)
    {
      result = false;
      break;
    }
  }

  return result;
}

const ScAddrVector & TemplateSearcherAbstract::getParams() const
{
  return params;
}

void TemplateSearcherAbstract::addParam(ScAddr const & param)
{
  params.push_back(param);
}

void TemplateSearcherAbstract::setArguments(ScAddr const & otherArguments)
{
  arguments = otherArguments;
}
