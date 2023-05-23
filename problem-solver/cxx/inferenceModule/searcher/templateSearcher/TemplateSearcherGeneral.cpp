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
    std::set<std::string> const & varNames,
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
          [&templateParams, &result, &varNames](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
            // Add search result items to the result Replacements
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
  std::map<std::string, std::string> linksContentMap = getTemplateKeyLinksContent(templateAddr);
  std::set<std::string> varNames;
  getVarNames(templateAddr, varNames);

  context->HelperSmartSearchTemplate(
      searchTemplate,
      [templateParams, &result, &varNames](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
        // Add search result items to the result Replacements
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
        // Filter result item by the same content
        return isContentIdentical(item, linksContentMap);
      });
}

std::map<std::string, std::string> TemplateSearcherGeneral::getTemplateKeyLinksContent(const ScAddr & templateAddr)
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
      if (context->GetLinkContent(linkAddr, stringContent))
      {
        linksContent.emplace(context->HelperGetSystemIdtf(linkAddr), stringContent);
      }
    }
  }
  return linksContent;
}
