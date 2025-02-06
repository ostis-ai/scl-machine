/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateSearcherOnlyMembershipArcsInStructures.hpp"

#include "inference/inference_keynodes.hpp"

namespace inference
{

TemplateSearcherOnlyMembershipArcsInStructures::TemplateSearcherOnlyMembershipArcsInStructures(
    ScMemoryContext * context,
    ScAddrUnorderedSet const & otherInputStructures)
  : TemplateSearcherInStructures(context, otherInputStructures)
{
}

TemplateSearcherOnlyMembershipArcsInStructures::TemplateSearcherOnlyMembershipArcsInStructures(
    ScMemoryContext * context)
  : TemplateSearcherOnlyMembershipArcsInStructures(context, {})
{
}

std::map<std::string, std::string> TemplateSearcherOnlyMembershipArcsInStructures::getTemplateLinksContent(
    ScAddr const & templateAddr)
{
  // TODO(kilativ-dotcom): need to decide what to return here. Input structures contain only membership arcs so there
  //  are no links in them and map should be empty?
  return {};
}

bool TemplateSearcherOnlyMembershipArcsInStructures::isValidElement(ScAddr const & element) const
{
  if (!context->GetElementType(element).IsMembershipArc())
    return true;
  auto const & structuresIterator =
      context->CreateIterator3(ScType::ConstNodeStructure, ScType::ConstPermPosArc, element);
  while (structuresIterator->Next())
  {
    if (inputStructures.count(structuresIterator->Get(0)))
      return true;
  }
  return false;
}
}  // namespace inference
