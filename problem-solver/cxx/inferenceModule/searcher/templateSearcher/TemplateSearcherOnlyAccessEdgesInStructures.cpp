#include "keynodes/InferenceKeynodes.hpp"

#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "TemplateSearcherOnlyAccessEdgesInStructures.hpp"

namespace inference
{

TemplateSearcherOnlyAccessEdgesInStructures::TemplateSearcherOnlyAccessEdgesInStructures(
    ScMemoryContext * context,
    ScAddrVector const & otherInputStructures)
  : TemplateSearcherInStructures(context, otherInputStructures)
{
}

TemplateSearcherOnlyAccessEdgesInStructures::TemplateSearcherOnlyAccessEdgesInStructures(ScMemoryContext * context)
  : TemplateSearcherOnlyAccessEdgesInStructures(context, {})
{
}
void TemplateSearcherOnlyAccessEdgesInStructures::searchTemplate(
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams,
    ScAddrHashSet const & variables,
    Replacements & result)
{
  searchWithoutContentResult = std::make_unique<ScTemplateSearchResult>();
  ScTemplate searchTemplate;
  if (context->HelperBuildTemplate(searchTemplate, templateAddr, templateParams))
  {
    this->contentOfAllInputStructures->clear();
    SC_LOG_DEBUG("start input structures processing");
    for (auto const & inputStructure : inputStructures)
    {
      ScAddrVector const & edges =
          utils::IteratorUtils::getAllWithType(context, inputStructure, ScType::EdgeAccess);
      contentOfAllInputStructures->insert(edges.cbegin(), edges.cend());
    }
    SC_LOG_DEBUG("input structures processed, found " << contentOfAllInputStructures->size() << " edges");
    if (context->HelperCheckEdge(
            InferenceKeynodes::concept_template_with_links, templateAddr, ScType::EdgeAccessConstPosPerm))
    {
      searchTemplateWithContent(searchTemplate, templateAddr, templateParams, result);
    }
    else
    {
      context->HelperSmartSearchTemplate(
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
            // Filter result item by not being Access Edge or belonging to any of the input structures
            return !context->GetElementType(item).BitAnd(ScType::EdgeAccess) ||
                   contentOfAllInputStructures->count(item);
          });
    }
  }
  else
  {
    throw std::runtime_error("Template is not built.");
  }
}

void TemplateSearcherOnlyAccessEdgesInStructures::searchTemplateWithContent(
    ScTemplate const & searchTemplate,
    ScAddr const & templateAddr,
    ScTemplateParams const & templateParams,
    Replacements & result)
{
  ScAddrHashSet variables;
  getVariables(templateAddr, variables);
  std::map<std::string, std::string> linksContentMap = getTemplateLinksContent(templateAddr);

  context->HelperSearchTemplate(
      searchTemplate,
      [templateParams, &result, &variables](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest {
        // Add search result item to the answer container
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
        // Filter result item by the same content and belonging to any of the input structures
        if (!isContentIdentical(item, linksContentMap))
          return false;
        for (size_t i = 0; i < item.Size(); i++)
        {
          if (context->GetElementType(item[i]).BitAnd(ScType::EdgeAccess) &&
              !contentOfAllInputStructures->count(item[i]))
            return false;
        }
        return true;
      });
}

map<std::string, std::string> TemplateSearcherOnlyAccessEdgesInStructures::getTemplateLinksContent(
    ScAddr const & templateAddr)
{
  //TODO(kilativ-dotcom): need to decide what to return here. Input structures contain only access edges so there are
  // no links in them and map should be empty?
  return {};
}
}  // namespace inference
