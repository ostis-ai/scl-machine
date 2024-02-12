/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherGeneral.hpp"

#include <memory>
#include <algorithm>

#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

TemplateSearcherGeneral::TemplateSearcherGeneral(ScMemoryContext * context)
  : TemplateSearcherAbstract(context)
{
}

void TemplateSearcherGeneral::searchTemplate(
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams,
    ScAddrHashSet const & variables,
    Replacements & result)
{
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
          [&templateParams, &result, &variables, this](
              ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
            // Add search result items to the result Replacements
            for (ScAddr const & variable : variables)
            {
              ScAddr argument;
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
          });
    }
  }
  else
  {
    throw runtime_error("Template is not built.");
  }
}

void TemplateSearcherGeneral::searchTemplateWithContent(
    ScTemplate const & searchTemplate,
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams,
    Replacements & result)
{
  std::map<std::string, std::string> linksContentMap = getTemplateLinksContent(templateAddr);
  ScAddrHashSet variables;
  getVariables(templateAddr, variables);

  context->HelperSmartSearchTemplate(
      searchTemplate,
      [templateParams, &result, &variables](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
        // Add search result items to the result Replacements
        for (ScAddr const & variable : variables)
        {
          ScAddr argument;
          if (item.Has(variable))
          {
            result[variable].push_back(item[variable]);
          }
          if (templateParams.Get(variable, argument))
          {
            result[variable].push_back(argument);
          }
        }
        return ScTemplateSearchRequest::STOP;
      },
      [&linksContentMap, this](ScTemplateSearchResultItem const & item) -> bool {
        // Filter result item by the same content
        return isContentIdentical(item, linksContentMap);
      });
}

std::map<std::string, std::string> TemplateSearcherGeneral::getTemplateLinksContent(ScAddr const & templateAddr)
{
  std::map<std::string, std::string> linksContent;
  ScIterator3Ptr linksIterator = context->Iterator3(templateAddr, ScType::EdgeAccessConstPosPerm, ScType::Link);
  while (linksIterator->Next())
  {
    ScAddr const & linkAddr = linksIterator->Get(2);
    std::string stringContent;
    if (context->GetLinkContent(linkAddr, stringContent))
    {
      linksContent.emplace(to_string(linkAddr.Hash()), stringContent);
    }
  }
  return linksContent;
}
