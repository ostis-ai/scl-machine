/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherOnlyAccessEdgesInStructures.hpp"

#include "keynodes/InferenceKeynodes.hpp"

namespace inference
{

TemplateSearcherOnlyAccessEdgesInStructures::TemplateSearcherOnlyAccessEdgesInStructures(
    ScMemoryContext * context,
    ScAddrUnorderedSet const & otherInputStructures)
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
  // TODO(kilativ-dotcom): need to decide what to return here. Input structures contain only access edges so there are
  //  no links in them and map should be empty?
  return {};
}

bool TemplateSearcherOnlyAccessEdgesInStructures::isValidElement(ScAddr const & element) const
{
  if (!context->GetElementType(element).BitAnd(ScType::EdgeAccess))
    return true;
  auto const & structuresIterator =
      context->Iterator3(ScType::NodeConstStruct, ScType::EdgeAccessConstPosPerm, element);
  while (structuresIterator->Next())
  {
    if (inputStructures.count(structuresIterator->Get(0)))
      return true;
  }
  return false;
}
}  // namespace inference
