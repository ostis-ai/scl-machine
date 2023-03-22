/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherInStructures.hpp"

#include <memory>
#include <algorithm>

#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

TemplateSearcherInStructures::TemplateSearcherInStructures(ScMemoryContext * context, ScAddr const & otherInputStructures)
    : TemplateSearcherAbstract(context)
{
  inputStructures = otherInputStructures;
}

TemplateSearcherInStructures::TemplateSearcherInStructures(ScMemoryContext * context)
    : TemplateSearcherAbstract(context) { }

std::vector<ScTemplateSearchResultItem> TemplateSearcherInStructures::searchTemplate(
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams)
{
  searchWithoutContentResult = std::make_unique<ScTemplateSearchResult>();
  std::vector<ScTemplateSearchResultItem> searchResultItems;
  ScAddrVector const & inputStructuresVector = utils::IteratorUtils::getAllWithType(context, inputStructures, ScType::Node);
  ScTemplate searchTemplate;
  if (context->HelperBuildTemplate(searchTemplate, templateAddr, templateParams))
  {
    if (context->HelperCheckEdge(
          InferenceKeynodes::concept_template_with_links, templateAddr, ScType::EdgeAccessConstPosPerm))
    {
      searchResultItems = searchTemplateWithContent(searchTemplate, templateAddr);
    }
    else
    {
      context->HelperSearchTemplate(
            searchTemplate,
            [&searchResultItems](ScTemplateSearchResultItem const & item) -> void {
              // Add search result item to the answer container
              searchResultItems.push_back(item);
            },
            [&inputStructuresVector, this](ScAddr const & item) -> bool {
              // Filter result item belonging to any of the input structures
              return std::any_of(
                    inputStructuresVector.cbegin(),
                    inputStructuresVector.cend(),
                    [&item, this](ScAddr const & structure) -> bool {
                      return context->HelperCheckEdge(structure, item, ScType::EdgeAccessConstPosPerm);
                    });
            });
    }
  }
  else
  {
    throw std::runtime_error("Template is not built.");
  }

  return searchResultItems;
}

std::vector<ScTemplateSearchResultItem> TemplateSearcherInStructures::searchTemplateWithContent(
      ScTemplate const & searchTemplate,
      ScAddr const & templateAddr)
{
  ScAddrVector const & inputStructuresVector = utils::IteratorUtils::getAllWithType(context, inputStructures, ScType::Node);
  std::vector<ScTemplateSearchResultItem> searchResultItems;
  std::map<std::string, std::string> linksContentMap = getTemplateKeyLinksContent(templateAddr);
  std::vector<ScTemplateSearchResultItem> searchWithContentResult;

  context->HelperSearchTemplate(
        searchTemplate,
        [&searchResultItems](ScTemplateSearchResultItem const & item) -> void {
          // Add search result item to the answer container
          searchResultItems.push_back(item);
        },
        [&inputStructuresVector, this](ScAddr const & item) -> bool {
          // Filter result item belonging to any of the input structures
          return std::any_of(
                inputStructuresVector.cbegin(),
                inputStructuresVector.cend(),
                [&item, this](ScAddr const & structure) -> bool {
                  return context->HelperCheckEdge(structure, item, ScType::EdgeAccessConstPosPerm);
                });
        });

  for (ScTemplateSearchResultItem searchResultItem : searchResultItems)
  {
    bool contentIsIdentical = true;

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


std::map<std::string, std::string> TemplateSearcherInStructures::getTemplateKeyLinksContent(const ScAddr & templateAddr)
{
  std::string const LINK_ALIAS = "_link";

  ScAddrVector const & inputStructuresVector = utils::IteratorUtils::getAllWithType(context, inputStructures, ScType::Node);
  std::map<std::string, std::string> linksContent;
  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        templateAddr,
        ScType::EdgeAccessVarPosPerm,
        ScType::Link >> LINK_ALIAS,
        ScType::EdgeAccessVarPosPerm,
        scAgentsCommon::CoreKeynodes::rrel_key_sc_element);

  context->HelperSearchTemplate(
        scTemplate,
        [this, &LINK_ALIAS, &linksContent](ScTemplateSearchResultItem const & item) -> void {
          ScAddr const & linkAddr = item[LINK_ALIAS];
          if (utils::CommonUtils::checkType(context, linkAddr, ScType::LinkVar))
          {
            std::string stringContent;
            ScStreamPtr linkContentStream = context->GetLinkContent(linkAddr);
            if (linkContentStream != nullptr)
              ScStreamConverter::StreamToString(linkContentStream, stringContent);
            linksContent.emplace(context->HelperGetSystemIdtf(linkAddr), stringContent);
          }
        },
        [&inputStructuresVector, this](ScAddr const & item) -> bool {
          // Filter result item belonging to any of the input structures
          return std::any_of(
                inputStructuresVector.cbegin(),
                inputStructuresVector.cend(),
                [&item, this](ScAddr const & structure) -> bool {
                  return context->HelperCheckEdge(structure, item, ScType::EdgeAccessConstPosPerm);
                });
        });

  return linksContent;
}
