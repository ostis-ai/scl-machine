/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

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

map<std::string, std::string> TemplateSearcherOnlyAccessEdgesInStructures::getTemplateLinksContent(
    ScAddr const & templateAddr)
{
  //TODO(kilativ-dotcom): need to decide what to return here. Input structures contain only access edges so there are
  // no links in them and map should be empty?
  return {};
}

void TemplateSearcherOnlyAccessEdgesInStructures::prepareBeforeSearch()
{
  this->contentOfAllInputStructures->clear();
  if (replacementsUsingType == REPLACEMENTS_ALL)
  {
    SC_LOG_DEBUG("start input structures processing");
    for (auto const & inputStructure : inputStructures)
    {
      ScAddrVector const & edges =
          utils::IteratorUtils::getAllWithType(context, inputStructure, ScType::EdgeAccess);
      contentOfAllInputStructures->insert(edges.cbegin(), edges.cend());
    }
    SC_LOG_DEBUG("input structures processed, found " << contentOfAllInputStructures->size() << " edges");
  }
}

bool TemplateSearcherOnlyAccessEdgesInStructures::isValidElement(ScAddr const & element) const
{
  if (replacementsUsingType == REPLACEMENTS_ALL)
    return !context->GetElementType(element).BitAnd(ScType::EdgeAccess) || contentOfAllInputStructures->count(element);
  else
    return !context->GetElementType(element).BitAnd(ScType::EdgeAccess) ||
           std::any_of(
               inputStructures.cbegin(), inputStructures.cend(), [&element, this](ScAddr const & inputStructure) {
                 return context->HelperCheckEdge(inputStructure, element, ScType::EdgeAccessConstPosPerm);
               });
}
}  // namespace inference
