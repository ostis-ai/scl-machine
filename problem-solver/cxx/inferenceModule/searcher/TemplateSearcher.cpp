/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcher.hpp"

#include <memory>
#include <algorithm>

#include <sc-agents-common/utils/CommonUtils.hpp>
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

TemplateSearcher::TemplateSearcher(ScMemoryContext * context)
  : TemplateSearcherAbstract(context) { }

vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplate(
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams)
{
  searchWithoutContentResult = std::make_unique<ScTemplateSearchResult>();
  vector<ScTemplateSearchResultItem> searchResultItems;
  ScTemplate searchTemplate;
  if (context->HelperBuildTemplate(searchTemplate, templateAddr, templateParams))
  {
    if (context->HelperCheckEdge(
            InferenceKeynodes::concept_template_with_links, templateAddr, ScType::EdgeAccessConstPosPerm))
      searchResultItems = searchTemplateWithContent(searchTemplate, templateAddr);
    else
    {
      context->HelperSearchTemplate(searchTemplate, *searchWithoutContentResult);
      for (size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
        searchResultItems.push_back((*searchWithoutContentResult)[searchItemIndex]);
    }
  }
  else
  {
    throw runtime_error("Template is not built.");
  }

  return searchResultItems;
}

std::vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplateWithContent(
    ScTemplate const & searchTemplate,
    ScAddr const & templateAddr)
{
  context->HelperSearchTemplate(searchTemplate, *searchWithoutContentResult);
  std::map<std::string, std::string> linksContentMap = getTemplateKeyLinksContent(templateAddr);
  std::vector<ScTemplateSearchResultItem> searchWithContentResult;
  for (size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
  {
    bool contentIsIdentical = true;

    ScTemplateSearchResultItem searchResultItem = (*searchWithoutContentResult)[searchItemIndex];
    for (auto const & linkIdContentPair : linksContentMap)
    {
      ScAddr linkAddr = searchResultItem[linkIdContentPair.first];
      std::string stringContent;
      ScStreamPtr linkContentStream = context->GetLinkContent(linkAddr);
      if (linkContentStream != nullptr)
        ScStreamConverter::StreamToString(linkContentStream, stringContent);
      if (stringContent != linkIdContentPair.second)
      {
        contentIsIdentical = false;
        break;
      }
    }
    if (contentIsIdentical)
      searchWithContentResult.push_back(searchResultItem);
  }

  return searchWithContentResult;
}

std::map<std::string, std::string> TemplateSearcher::getTemplateKeyLinksContent(const ScAddr & templateAddr)
{
  std::string const LINK_ALIAS = "_link";

  std::map<std::string, std::string> linksContent;
  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        templateAddr,
        ScType::EdgeAccessVarPosPerm,
        ScType::Link >> LINK_ALIAS,
        ScType::EdgeAccessVarPosPerm,
        scAgentsCommon::CoreKeynodes::rrel_key_sc_element);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);
  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    ScAddr const & linkAddr = searchResult[i][LINK_ALIAS];
    if (utils::CommonUtils::checkType(context, linkAddr, ScType::LinkVar))
    {
      std::string stringContent;
      ScStreamPtr linkContentStream = context->GetLinkContent(linkAddr);
      if (linkContentStream != nullptr)
        ScStreamConverter::StreamToString(linkContentStream, stringContent);
      linksContent.emplace(context->HelperGetSystemIdtf(linkAddr), stringContent);
    }
  }
  return linksContent;
}

Replacements TemplateSearcher::searchTemplate(
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
