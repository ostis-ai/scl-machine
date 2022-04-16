/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <algorithm>
#include <boost/locale.hpp>

#include "TemplateSearcher.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

#include "keynodes/Keynodes.hpp"

using namespace rulesApplicationModule;
using namespace std;

TemplateSearcher::TemplateSearcher(ScMemoryContext * context)
{
  this->context = context;
}

vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplateInStructure(
      ScAddr const & templateAddr,
      ScAddr const & structure,
      ScTemplateParams const & templateParams)
{
  searchWithoutContentResult = make_unique<ScTemplateSearchResult>();
  vector<ScTemplateSearchResultItem> searchResult;
  ScTemplate searchTemplate;
  if (!context->HelperBuildTemplate(searchTemplate, templateAddr, templateParams))
    throw runtime_error("Template is not built.");

  if (context->HelperCheckEdge( Keynodes::concept_template_with_links, templateAddr, ScType::EdgeAccessConstPosPerm))
    searchResult = searchTemplateWithContentInStructure(searchTemplate, structure, templateAddr);
  else
  {
    context->HelperSearchTemplateInStruct(searchTemplate, structure, *searchWithoutContentResult);
    for (size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
      searchResult.push_back((*searchWithoutContentResult)[searchItemIndex]);
  }

  return searchResult;
}

vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplateWithContentInStructure(
      ScTemplate const & searchTemplate,
      ScAddr const & structure,
      ScAddr const & templateAddr)
{
  context->HelperSearchTemplateInStruct(searchTemplate, structure, *searchWithoutContentResult);
  map<string, string> linksContentMap = getTemplateKeyLinksContent(templateAddr);
  vector<ScTemplateSearchResultItem> searchWithContentResult;
  for (size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
  {
    bool isContentIdentical = true;

    ScTemplateSearchResultItem searchResultItem = (*searchWithoutContentResult)[searchItemIndex];
    for (auto const & linkIdContentPair: linksContentMap)
    {
      ScAddr linkAddr = searchResultItem[linkIdContentPair.first];
      if (!equalsIgnoreCase(getLinkContent(linkAddr), linkIdContentPair.second))
      {
        isContentIdentical = false;
        break;
      }
    }
    if (isContentIdentical)
      searchWithContentResult.push_back(searchResultItem);
  }

  return searchWithContentResult;
}

map<string, string> TemplateSearcher::getTemplateKeyLinksContent(ScAddr const & templateAddr)
{
  string link_alias = "_link";

  map<string, string> linksContent;
  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        templateAddr,
        ScType::EdgeAccessVarPosPerm,
        ScType::Link >> link_alias,
        ScType::EdgeAccessVarPosPerm,
        scAgentsCommon::CoreKeynodes::rrel_key_sc_element
  );
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);
  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    ScAddr linkAddr = searchResult[i][link_alias];
    if (utils::CommonUtils::checkType(context, linkAddr, ScType::LinkVar))
      linksContent.emplace(context->HelperGetSystemIdtf(linkAddr), getLinkContent(linkAddr));
  }

  return linksContent;
}

string TemplateSearcher::getLinkContent(ScAddr const & linkAddr)
{
  // Don't use ScLink class to get content, unpredictable behaviour
  // https://trello.com/c/IcQc46kh
  string linkContent;
  ScStreamPtr linkContentStream = context->GetLinkContent(linkAddr);
  if (linkContentStream != nullptr)
    ScStreamConverter::StreamToString(linkContentStream, linkContent);
  else
    linkContent = "";
  return linkContent;
}

//boost::iequals don't work with cyrillic symbols
bool TemplateSearcher::equalsIgnoreCase(string const & first, string const & second)
{
  boost::locale::generator gen;
  std::locale::global(gen(""));

  return boost::locale::to_lower(first) == boost::locale::to_lower(second);
}
