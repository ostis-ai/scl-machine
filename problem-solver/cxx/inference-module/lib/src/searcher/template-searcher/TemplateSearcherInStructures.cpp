/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherInStructures.hpp"

#include <memory>
#include <algorithm>

#include <sc-agents-common/utils/CommonUtils.hpp>

#include "inference/inference_keynodes.hpp"

using namespace inference;

TemplateSearcherInStructures::TemplateSearcherInStructures(
    ScMemoryContext * context,
    ScAddrUnorderedSet const & otherInputStructures)
  : TemplateSearcherAbstract(context)
{
  inputStructures = otherInputStructures;
}

TemplateSearcherInStructures::TemplateSearcherInStructures(ScMemoryContext * context)
  : TemplateSearcherInStructures(context, {})
{
}

void TemplateSearcherInStructures::searchTemplate(
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams,
    ScAddrUnorderedSet const & variables,
    Replacements & result)
{
  ScTemplate searchTemplate;
  context->BuildTemplate(searchTemplate, templateAddr, templateParams);
  if (context->CheckConnector(InferenceKeynodes::concept_template_with_links, templateAddr, ScType::ConstPermPosArc))
  {
    searchTemplateWithContent(searchTemplate, templateAddr, templateParams, result);
  }
  else
  {
    context->SearchByTemplateInterruptibly(
        searchTemplate,
        [templateParams, &result, &variables, this](
            ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
          // Add search result item to the answer container
          ScAddr argument;
          for (ScAddr const & variable : variables)
          {
            if (item.Has(variable))
            {
              result[variable].push_back(item[variable]);
            }
            else if (templateParams.Get(variable, argument))
            {
              result[variable].push_back(argument);
            }
          }
          if (replacementsUsingType == ReplacementsUsingType::REPLACEMENTS_FIRST)
            return ScTemplateSearchRequest::STOP;
          else
            return ScTemplateSearchRequest::CONTINUE;
        },
        [this](ScAddr const & item) -> bool {
          // Filter result item belonging to any of the input structures
          return isValidElement(item);
        });
  }
}

void TemplateSearcherInStructures::searchTemplateWithContent(
    ScTemplate const & searchTemplate,
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams,
    Replacements & result)
{
  ScAddrUnorderedSet variables;
  getVariables(templateAddr, variables);
  std::map<std::string, std::string> linksContentMap = getTemplateLinksContent(templateAddr);

  context->SearchByTemplate(
      searchTemplate,
      [templateParams, &result, &variables, this](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
        // Add search result item to the answer container
        for (ScAddr const & variable : variables)
        {
          ScAddr argument;
          if (item.Get(variable, argument) || templateParams.Get(variable, argument))
          {
            result[variable].push_back(argument);
          }
        }
        if (replacementsUsingType == ReplacementsUsingType::REPLACEMENTS_FIRST)
          return ScTemplateSearchRequest::STOP;
        else
          return ScTemplateSearchRequest::CONTINUE;
      },
      [&linksContentMap, this](ScTemplateSearchResultItem const & item) -> bool {
        // Filter result item by the same content and belonging to any of the input structures
        if (!isContentIdentical(item, linksContentMap))
          return false;
        for (size_t i = 0; i < item.Size(); i++)
        {
          ScAddr const & checkedElement = item[i];
          if (isValidElement(checkedElement) == SC_FALSE)
            return false;
        }
        return true;
      });
}

std::map<std::string, std::string> TemplateSearcherInStructures::getTemplateLinksContent(ScAddr const & templateAddr)
{
  std::map<std::string, std::string> linksContent;
  ScIterator3Ptr const & linksIterator =
      context->CreateIterator3(templateAddr, ScType::ConstPermPosArc, ScType::NodeLink);
  while (linksIterator->Next())
  {
    ScAddr const & linkAddr = linksIterator->Get(2);
    std::string stringContent;
    if (isValidElement(linkAddr))
    {
      context->GetLinkContent(linkAddr, stringContent);
      linksContent.emplace(std::to_string(linkAddr.Hash()), stringContent);
    }
  }

  return linksContent;
}

bool TemplateSearcherInStructures::isValidElement(ScAddr const & element) const
{
  auto const & structuresIterator =
      context->CreateIterator3(ScType::ConstNodeStructure, ScType::ConstPermPosArc, element);
  while (structuresIterator->Next())
  {
    if (inputStructures.count(structuresIterator->Get(0)))
      return true;
  }
  return false;
}
