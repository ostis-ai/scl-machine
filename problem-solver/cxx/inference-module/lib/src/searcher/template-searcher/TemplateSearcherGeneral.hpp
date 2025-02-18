/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

#include "inference/replacements_utils.hpp"

#include "TemplateSearcherAbstract.hpp"

namespace inference
{
class TemplateSearcherGeneral : public TemplateSearcherAbstract
{
public:
  explicit TemplateSearcherGeneral(ScMemoryContext * ms_context);

  void searchTemplate(
      ScAddr const & templateAddr,
      ScTemplateParams const & templateParams,
      ScAddrUnorderedSet const & variables,
      Replacements & result) override;

private:
  void searchTemplateWithContent(
      ScTemplate const & searchTemplate,
      ScAddr const & templateAddr,
      ScTemplateParams const & templateParams,
      Replacements & result) override;

  std::map<std::string, std::string> getTemplateLinksContent(ScAddr const & templateAddr) override;
};
}  // namespace inference
