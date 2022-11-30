/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include <sc-memory/sc_memory.hpp>

#include "comparator/AddrComparator.hpp"

namespace inference
{
class TemplateManager
{
public:
  explicit TemplateManager(ScMemoryContext * ms_context);

  std::vector<ScTemplateParams> createTemplateParamsList(ScAddr const & scTemplate, ScAddrVector const & argumentList);

  std::vector<ScTemplateParams> createTemplateParams(ScAddr const & scTemplate, ScAddrVector const & argumentList);

  ~TemplateManager() = default;

private:
  ScMemoryContext * context;
};
}  // namespace inference
