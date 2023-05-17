/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "SolutionTreeManagerAbstract.hpp"

namespace inference
{
/// Solution tree that doesn't generate nodes, only output structure. Need to optimize inference
class SolutionTreeManagerEmpty : public SolutionTreeManagerAbstract
{
public:
  explicit SolutionTreeManagerEmpty(ScMemoryContext * context);

  bool addNode(ScAddr const & formula, Replacements const & replacements) override;
};

}  // namespace inference
