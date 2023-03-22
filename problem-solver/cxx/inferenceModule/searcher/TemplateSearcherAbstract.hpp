/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>
#include <algorithm>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

#include <sc-agents-common/utils/CommonUtils.hpp>

#include "../utils/ReplacementsUtils.hpp"

namespace inference
{
class TemplateSearcherAbstract
{
public:
  explicit TemplateSearcherAbstract(ScMemoryContext * context);

  virtual ~TemplateSearcherAbstract() = default;

  virtual std::vector<ScTemplateSearchResultItem> searchTemplate(
      ScAddr const & templateAddr,
      ScTemplateParams const & templateParams) = 0;

  virtual Replacements searchTemplate(
        ScAddr const & templateAddr,
        vector<ScTemplateParams> const & scTemplateParamsVector);

  Replacements searchTemplate(ScAddr const & templateAddr);

  std::set<std::string> getVarNames(ScAddr const & structure);

  const ScAddrVector & getParams() const;

  void addParam(ScAddr const & param);

  bool addParamIfNotPresent(ScAddr const & param);

  void setArguments(ScAddr const & otherArguments);

  void setInputStructures(ScAddr const & otherInputStructures);

protected:
  virtual std::vector<ScTemplateSearchResultItem> searchTemplateWithContent(
      ScTemplate const & searchTemplate,
      ScAddr const & templateAddr) = 0;

  virtual std::map<std::string, std::string> getTemplateKeyLinksContent(ScAddr const & templateAddr) = 0;

  ScMemoryContext * context;
  std::unique_ptr<ScTemplateSearchResult> searchWithoutContentResult;
  ScAddrVector params;
  ScAddr arguments;
  ScAddr inputStructures;
};
}  // namespace inference
