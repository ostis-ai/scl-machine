/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "FormulasIterationStrategyAbstract.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "logic/LogicExpressionNode.hpp"

namespace inference
{
// Стратегия обхода формул с проверкой условия достижения цели (стандартная реализация, которая была всегда)
class FormulasIterationStrategyTarget : public FormulasIterationStrategyAbstract
{
public:
  explicit FormulasIterationStrategyTarget(ScMemoryContext * context);

  void setTargetStructure(ScAddr const & otherTargetStructure);

  bool applyIterationStrategy(ScAddr const & formulasSet, ScAddr const & outputStructure) override;

  bool isTargetAchieved(ScAddrVector const & argumentVector);

protected:
  ScAddr targetStructure;
};
}
