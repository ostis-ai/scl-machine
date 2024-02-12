/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "TemplateSearcherInStructures.hpp"

namespace inference
{

class TemplateSearcherOnlyAccessEdgesInStructures : public TemplateSearcherInStructures
{
public:
  explicit TemplateSearcherOnlyAccessEdgesInStructures(
      ScMemoryContext * context,
      ScAddrVector const & otherInputStructures);

  explicit TemplateSearcherOnlyAccessEdgesInStructures(ScMemoryContext * ms_context);

private:
  map<std::string, std::string> getTemplateLinksContent(ScAddr const & templateAddr) override;

  void prepareBeforeSearch() override;

  bool isValidElement(ScAddr const & element) const override;
};

}  // namespace inference
