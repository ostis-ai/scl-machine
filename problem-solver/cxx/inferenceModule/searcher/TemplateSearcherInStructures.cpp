/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherInStructures.hpp"

#include <memory>
#include <algorithm>
#include <chrono>

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

Replacements TemplateSearcherInStructures::searchTemplate(
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams)
{
  Replacements result;
  searchWithoutContentResult = std::make_unique<ScTemplateSearchResult>();
  ScAddrVector const & inputStructuresVector = utils::IteratorUtils::getAllWithType(context, inputStructures, ScType::Node);
  ScTemplate searchTemplate;
  if (context->HelperBuildTemplate(searchTemplate, templateAddr, templateParams))
  {
    if (context->HelperCheckEdge(
          InferenceKeynodes::concept_template_with_links, templateAddr, ScType::EdgeAccessConstPosPerm))
    {
      result = searchTemplateWithContent(searchTemplate, templateAddr, templateParams);
    }
    else
    {
      std::set<std::string> const & varNames = getVarNames(templateAddr);
       context->HelperSmartSearchTemplate(
            searchTemplate,
            [templateParams, &result, &varNames](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
              // Add search result item to the answer container
              for (std::string const & varName : varNames)
              {
                ScAddr argument;
                if (item.Has(varName))
                {
                  result[varName].push_back(item[varName]);
                }
                if (templateParams.Get(varName, argument))
                {
                  result[varName].push_back(argument);
                }
              }
              return ScTemplateSearchRequest::STOP;
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

  return result;
}

Replacements TemplateSearcherInStructures::searchTemplateWithContent(
      ScTemplate const & searchTemplate,
      ScAddr const & templateAddr,
      ScTemplateParams const & templateParams)
{
  Replacements result;
  ScAddrVector const & inputStructuresVector = utils::IteratorUtils::getAllWithType(context, inputStructures, ScType::Node);
  std::set<std::string> const & varNames = getVarNames(templateAddr);
  std::map<std::string, std::string> linksContentMap = getTemplateKeyLinksContent(templateAddr);

  context->HelperSearchTemplate(
        searchTemplate,
        [templateParams, &result, &varNames](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
          // Add search result item to the answer container
          for (std::string const & varName : varNames)
          {
            ScAddr argument;
            if (item.Has(varName))
            {
              result[varName].push_back(item[varName]);
            }
            if (templateParams.Get(varName, argument))
            {
              result[varName].push_back(argument);
            }
          }
          return ScTemplateSearchRequest::STOP;
        },
        [&inputStructuresVector, &linksContentMap, this](ScTemplateSearchResultItem const & item) -> bool {
          // Filter result item by the same content and belonging to any of the input structures
          bool contentIdentical = isContentIdentical(item, linksContentMap);
          bool isElementInStructures = std::any_of(
                inputStructuresVector.cbegin(),
                inputStructuresVector.cend(),
                [&item, this](ScAddr const & structure) -> bool {
                  bool result = true;
                  for (size_t i = 0; i < item.Size(); i++)
                  {
                    if (!context->HelperCheckEdge(structure, item[i], ScType::EdgeAccessConstPosPerm))
                    {
                      result = false;
                      break;
                    }
                  }
                  return result;
                });
          return contentIdentical && isElementInStructures;
        });

  return result;
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
