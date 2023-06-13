/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherInStructures.hpp"

#include <memory>
#include <algorithm>

#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

TemplateSearcherInStructures::TemplateSearcherInStructures(
    ScMemoryContext * context,
    ScAddrVector const & otherInputStructures)
  : TemplateSearcherAbstract(context)
{
  inputStructures = otherInputStructures;
}

TemplateSearcherInStructures::TemplateSearcherInStructures(ScMemoryContext * context)
  : TemplateSearcherAbstract(context)
{
}

void TemplateSearcherInStructures::searchTemplate(
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams,
    std::set<std::string> const & varNames,
    Replacements & result)
{
  searchWithoutContentResult = std::make_unique<ScTemplateSearchResult>();
  ScTemplate searchTemplate;
  if (context->HelperBuildTemplate(searchTemplate, templateAddr, templateParams))
  {
    if (context->HelperCheckEdge(
            InferenceKeynodes::concept_template_with_links, templateAddr, ScType::EdgeAccessConstPosPerm))
    {
      searchTemplateWithContent(searchTemplate, templateAddr, templateParams, result);
    }
    else
    {
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
          [this](ScAddr const & item) -> bool {
            // Filter result item belonging to any of the input structures
            return std::any_of(
                inputStructures.cbegin(), inputStructures.cend(), [&item, this](ScAddr const & structure) -> bool {
                  return context->HelperCheckEdge(structure, item, ScType::EdgeAccessConstPosPerm);
                });
          });
    }
  }
  else
  {
    throw std::runtime_error("Template is not built.");
  }
}

void TemplateSearcherInStructures::searchTemplateWithContent(
    ScTemplate const & searchTemplate,
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams,
    Replacements & result)
{
  std::set<std::string> varNames;
  getVarNames(templateAddr, varNames);
  std::map<std::string, std::string> linksContentMap = getTemplateLinksContent(templateAddr);

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
      [&linksContentMap, this](ScTemplateSearchResultItem const & item) -> bool {
        // Filter result item by the same content and belonging to any of the input structures
        bool contentIdentical = isContentIdentical(item, linksContentMap);
        bool isElementInStructures = std::any_of(
            inputStructures.cbegin(), inputStructures.cend(), [&item, this](ScAddr const & structure) -> bool {
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
}

std::map<std::string, std::string> TemplateSearcherInStructures::getTemplateLinksContent(ScAddr const & templateAddr)
{
  std::map<std::string, std::string> linksContent;
  ScIterator3Ptr const & linksIterator =
      context->Iterator3(templateAddr, ScType::EdgeAccessConstPosPerm, ScType::Link);
  while (linksIterator->Next())
  {
    ScAddr const & linkAddr = linksIterator->Get(2);
    std::string stringContent;
    if (std::any_of(
            inputStructures.cbegin(), inputStructures.cend(), [&linkAddr, this](ScAddr const & structure) -> bool {
              return context->HelperCheckEdge(structure, linkAddr, ScType::EdgeAccessConstPosPerm);
            }))
    {
      context->GetLinkContent(linkAddr, stringContent);
      linksContent.emplace(to_string(linkAddr.Hash()), stringContent);
    }
  }

  return linksContent;
}
