/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherAbstract.hpp"

#include "sc-agents-common/utils/CommonUtils.hpp"

using namespace inference;

TemplateSearcherAbstract::TemplateSearcherAbstract(ScMemoryContext * context)
  : context(context)
{
}

void TemplateSearcherAbstract::setInputStructures(ScAddrVector const & otherInputStructures)
{
  inputStructures = otherInputStructures;
}

ScAddrVector TemplateSearcherAbstract::getInputStructures() const
{
  return inputStructures;
}

void TemplateSearcherAbstract::searchTemplate(
    ScAddr const & templateAddr,
    vector<ScTemplateParams> const & scTemplateParamsVector,
    std::set<std::string> const & varNames,
    Replacements & result)
{
  Replacements searchResults;
  ScAddrVector varNameReplacementValues;
  ScAddr argument;

  for (ScTemplateParams const & scTemplateParams : scTemplateParamsVector)
  {
    searchTemplate(templateAddr, scTemplateParams, varNames, searchResults);
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
}

void TemplateSearcherAbstract::getVarNames(ScAddr const & formula, std::set<std::string> & varNames)
{
  ScIterator3Ptr const & formulaVariablesIterator =
      context->Iterator3(formula, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  ScAddr element;
  std::string variableSystemIdtf;
  while (formulaVariablesIterator->Next())
  {
    element = formulaVariablesIterator->Get(2);
    // TODO(MksmOrlov): replace with ScType::Var after new memory realisation
    if (context->GetElementType(element) == ScType::NodeVar || context->GetElementType(element) == ScType::LinkVar)
    {
      variableSystemIdtf = context->HelperGetSystemIdtf(element);
      if (!variableSystemIdtf.empty())
        varNames.insert(variableSystemIdtf);
    }
  }
}

bool TemplateSearcherAbstract::isContentIdentical(
    ScTemplateSearchResultItem const & item,
    std::map<std::string, std::string> const & linksContentMap)
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
