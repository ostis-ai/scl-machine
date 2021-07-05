/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "TemplateSearcher.hpp"

#include <memory>
#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

TemplateSearcher::TemplateSearcher(ScMemoryContext * context)
{
  this->context = context;
}

vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplate(
      const ScAddr & templateAddr,
      const ScTemplateParams & templateParams)
{
  searchWithoutContentResult = std::make_unique<ScTemplateSearchResult>();
  vector<ScTemplateSearchResultItem> searchResult;
  ScTemplate searchTemplate;
  if (context->HelperBuildTemplate(searchTemplate, templateAddr, templateParams))
  {
    if (context->HelperCheckEdge(
          InferenceKeynodes::concept_template_with_links,
          templateAddr,
          ScType::EdgeAccessConstPosPerm))
    {
      searchResult = searchTemplateWithContent(searchTemplate, templateAddr);
    }
    else
    {
      context->HelperSearchTemplate(searchTemplate, *searchWithoutContentResult);
      for(size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
      {
        searchResult.push_back((*searchWithoutContentResult)[searchItemIndex]);
      }
    }
  }
  else { throw runtime_error("Template is not built."); }

  return searchResult;
}

std::vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplateWithContent(
      const ScTemplate &searchTemplate,
      const ScAddr &templateAddr)
{
  context->HelperSearchTemplate(searchTemplate, *searchWithoutContentResult);
  std::map<std::string, std::string> linksContentMap =  getTemplateLinksContent(templateAddr);
  vector<ScTemplateSearchResultItem> searchWithContentResult;
  for(size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
  {
    bool contentIsIdentical = true;

    ScTemplateSearchResultItem searchResultItem = (*searchWithoutContentResult)[searchItemIndex];
    for (auto const & linkIdContentPair: linksContentMap)
    {
      ScAddr link = searchResultItem[linkIdContentPair.first];
      std::string stringContent;
      ScStreamConverter::StreamToString(context->GetLinkContent(link), stringContent);
      if (stringContent != linkIdContentPair.second)
      {
        contentIsIdentical = false;
        break;
      }
    }
    if (contentIsIdentical)
    {
      searchWithContentResult.push_back(searchResultItem);
    }
  }

  return searchWithContentResult;
}

std::map<std::string, std::string> TemplateSearcher::getTemplateLinksContent(const ScAddr &templateAddr)
{
  std::string link_alias = "_link";

  std::map<std::string, std::string> linksContent;
  ScTemplate scTemplate;
  scTemplate.Triple(templateAddr, ScType::EdgeAccessVarPosPerm, ScType::Link >> link_alias);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);
  for(size_t i = 0; i < searchResult.Size(); i++)
  {
    ScAddr link = searchResult[i][link_alias];
    if (utils::CommonUtils::checkType(context, link, ScType::LinkVar))
    {
      std::string stringContent;
      ScStreamConverter::StreamToString(context->GetLinkContent(link), stringContent);
      linksContent.emplace(context->HelperGetSystemIdtf(link), stringContent);
    }
  }
  return linksContent;
}
