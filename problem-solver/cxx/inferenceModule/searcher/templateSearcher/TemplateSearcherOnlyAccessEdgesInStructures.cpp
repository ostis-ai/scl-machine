/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "keynodes/InferenceKeynodes.hpp"

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

bool TemplateSearcherOnlyAccessEdgesInStructures::isValidElement(ScAddr const & element) const
{
  return !context->GetElementType(element).BitAnd(ScType::EdgeAccess) ||
         std::any_of(
             inputStructures.cbegin(), inputStructures.cend(), [&element, this](ScAddr const & inputStructure) -> bool {
               return context->HelperCheckEdge(inputStructure, element, ScType::EdgeAccessConstPosPerm);
             });
}
}  // namespace inference
