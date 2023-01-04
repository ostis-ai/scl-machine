/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace inference
{
class LogicRuleUtils
{
public:
  static ScAddr getFormulaPremise(ScMemoryContext * context, ScAddr const & logicalFormula);

  static ScAddr getFormulaConclusion(ScMemoryContext * context, ScAddr const & logicalFormula);
};

}  // namespace inference
