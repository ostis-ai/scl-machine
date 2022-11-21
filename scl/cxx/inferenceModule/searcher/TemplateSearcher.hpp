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
      ScAddr const & templateAddr,
      ScTemplateParams const & templateParams);

  std::map<std::string, std::vector<ScAddr>> searchTemplate(
      ScAddr const & templateAddr,
      std::vector<ScTemplateParams> const & scTemplateParamsVector);

  std::map<std::string, std::vector<ScAddr>> searchTemplate(ScAddr const & templateAddr);

  std::set<std::string> getVarNames(ScAddr const & structure);

  ScAddrVector const & getParams() const;

  void addParam(ScAddr const & param);

  bool addParamIfNotPresent(ScAddr const & param);

  void setInputStructure(ScAddr const & inputStructure);

private:
  std::vector<ScTemplateSearchResultItem> searchTemplateWithContent(
      ScTemplate const & searchTemplate,
      ScAddr const & templateAddr);

  std::map<std::string, std::string> getTemplateKeyLinksContent(ScAddr const & templateAddr);

  ScMemoryContext * context;
  std::unique_ptr<ScTemplateSearchResult> searchWithoutContentResult;
  ScAddrVector params;
  ScAddr inputStructure;
};
}  // namespace inference
