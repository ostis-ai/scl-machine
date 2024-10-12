/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "TemplateSearcherInStructures.hpp"

namespace inference
{

class TemplateSearcherOnlyMembershipArcsInStructures : public TemplateSearcherInStructures
{
public:
  explicit TemplateSearcherOnlyMembershipArcsInStructures(
      ScMemoryContext * context,
      ScAddrUnorderedSet const & otherInputStructures);

  explicit TemplateSearcherOnlyMembershipArcsInStructures(ScMemoryContext * ms_context);

private:
  std::map<std::string, std::string> getTemplateLinksContent(ScAddr const & templateAddr) override;

  bool isValidElement(ScAddr const & element) const override;
};

}  // namespace inference
