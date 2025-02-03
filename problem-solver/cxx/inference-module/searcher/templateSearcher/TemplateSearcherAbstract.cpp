/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherAbstract.hpp"

#include "sc-agents-common/utils/CommonUtils.hpp"

using namespace inference;

TemplateSearcherAbstract::TemplateSearcherAbstract(
    ScMemoryContext * context,
    ReplacementsUsingType replacementsUsingType,
    OutputStructureFillingType outputStructureFillingType)
  : context(context)
  , replacementsUsingType(replacementsUsingType)
  , outputStructureFillingType(outputStructureFillingType)
{
}

void TemplateSearcherAbstract::setInputStructures(ScAddrUnorderedSet const & otherInputStructures)
{
  inputStructures = otherInputStructures;
}

ScAddrUnorderedSet TemplateSearcherAbstract::getInputStructures() const
{
  return inputStructures;
}

void TemplateSearcherAbstract::searchTemplate(
    ScAddr const & templateAddr,
    std::vector<ScTemplateParams> const & scTemplateParamsVector,
    ScAddrUnorderedSet const & variables,
    Replacements & result)
{
  Replacements searchResults;
  ScAddrVector variableReplacementValues;
  ScAddr argument;

  for (ScTemplateParams const & scTemplateParams : scTemplateParamsVector)
  {
    searchTemplate(templateAddr, scTemplateParams, variables, searchResults);
    for (ScAddr const & variable : variables)
    {
      if (searchResults.count(variable))
      {
        variableReplacementValues = searchResults.at(variable);
        if (scTemplateParams.Get(variable, argument))
        {
          result[variable].push_back(argument);
        }
        else if (!variableReplacementValues.empty())
        {
          result[variable] = variableReplacementValues;
        }
      }
    }
  }
}

void TemplateSearcherAbstract::getVariables(ScAddr const & formula, ScAddrUnorderedSet & variables)
{
  ScIterator3Ptr const & formulaVariablesIterator =
      context->CreateIterator3(formula, ScType::ConstPermPosArc, ScType::Var);
  while (formulaVariablesIterator->Next())
    variables.insert(formulaVariablesIterator->Get(2));
}

void TemplateSearcherAbstract::getConstants(ScAddr const & formula, ScAddrUnorderedSet & constants)
{
  ScIterator3Ptr const & formulaConstantsIterator =
      context->CreateIterator3(formula, ScType::ConstPermPosArc, ScType::Const);
  while (formulaConstantsIterator->Next())
    constants.insert(formulaConstantsIterator->Get(2));
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
