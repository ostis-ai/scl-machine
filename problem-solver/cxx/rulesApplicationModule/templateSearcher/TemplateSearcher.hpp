/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace rulesApplicationModule
{

using SearchWithContentResult = std::vector<ScTemplateSearchResultItem>;

class TemplateSearcher
{
public:
  explicit TemplateSearcher(ScMemoryContext * ms_context);

  SearchWithContentResult searchTemplateInStructure(
        ScAddr const & templateAddr,
        ScAddr const & structure,
        ScTemplateParams const & templateParams = ScTemplateParams());

private:
  SearchWithContentResult searchTemplateWithContentInStructure(
        ScTemplate const & searchTemplate,
        ScAddr const & structure,
        ScAddr const & templateAddr);

  std::map<std::string, std::string> getTemplateKeyLinksContent(ScAddr const & templateAddr);

  std::string getLinkContent(ScAddr const & linkAddr);

  bool equalsIgnoreCase(std::string const & first, std::string const & second);

  ScMemoryContext * context;
  std::unique_ptr<ScTemplateSearchResult> searchWithoutContentResult;
};

} // namespace rulesApplicationModule
