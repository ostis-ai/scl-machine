/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <queue>
#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace inference
{

class TemplateSearcher
{
public:
  explicit TemplateSearcher(ScMemoryContext * ms_context);

  std::vector<ScTemplateSearchResultItem> searchTemplate(
        const ScAddr & templateAddr,
        const ScTemplateParams & templateParams,
        const ScAddr & inputStructure = ScAddr());

private:
  std::vector<ScTemplateSearchResultItem> searchTemplateWithContent(
        const ScTemplate &searchTemplate,
        const ScAddr &templateAddr,
        const ScAddr & inputStructure = ScAddr());

  std::map<std::string, std::string> getTemplateKeyLinksContent(const ScAddr &templateAddr);

  ScMemoryContext *context;
  std::unique_ptr<ScTemplateSearchResult> searchWithoutContentResult;
};
}
