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
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

TemplateSearcher::TemplateSearcher(ScMemoryContext * context)
{
  this->context = context;
}

vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplate(
      const ScAddr & templateAddr,
      const ScTemplateParams & templateParams,
      const ScAddr & inputStructure)
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
      context->HelperSearchTemplateInStruct(searchTemplate, inputStructure, *searchWithoutContentResult);
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
      const ScTemplate & searchTemplate,
      const ScAddr & templateAddr,
      const ScAddr & inputStructure)
{
  context->HelperSearchTemplateInStruct(searchTemplate, inputStructure, *searchWithoutContentResult);
  std::map<std::string, std::string> linksContentMap = getTemplateKeyLinksContent(templateAddr);
  vector<ScTemplateSearchResultItem> searchWithContentResult;
  for(size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
  {
    bool contentIsIdentical = true;

    ScTemplateSearchResultItem searchResultItem = (*searchWithoutContentResult)[searchItemIndex];
    for (auto const & linkIdContentPair: linksContentMap)
    {
      ScAddr linkAddr = searchResultItem[linkIdContentPair.first];
      std::string stringContent;
      ScStreamPtr linkContentStream = context->GetLinkContent(linkAddr);
      if (linkContentStream != nullptr)
      {
        ScStreamConverter::StreamToString(linkContentStream, stringContent);
      }
      else
      {
        stringContent = "";
      }
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

std::map<std::string, std::string> TemplateSearcher::getTemplateKeyLinksContent(const ScAddr &templateAddr)
{
  std::string link_alias = "_link";

  std::map<std::string, std::string> linksContent;
  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        templateAddr,
        ScType::EdgeAccessVarPosPerm,
        ScType::Link >> link_alias,
        ScType::EdgeAccessVarPosPerm,
        scAgentsCommon::CoreKeynodes::rrel_key_sc_element);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);
  for(size_t i = 0; i < searchResult.Size(); i++)
  {
    ScAddr linkAddr = searchResult[i][link_alias];
    if (utils::CommonUtils::checkType(context, linkAddr, ScType::LinkVar))
    {
      std::string stringContent;
      ScStreamPtr linkContentStream = context->GetLinkContent(linkAddr);
      if (linkContentStream != nullptr)
      {
        ScStreamConverter::StreamToString(linkContentStream, stringContent);
      }
      else
      {
        stringContent = "";
      }
      linksContent.emplace(context->HelperGetSystemIdtf(linkAddr), stringContent);
    }
  }
  return linksContent;
}
