/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "TemplateExpressionNode.hpp"

using namespace inference;

class DisjunctionExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit DisjunctionExpressionNode(ScMemoryContext * context, OperandsVector & operands);

  void compute(LogicFormulaResult & result) const override;

  void generate(Replacements & replacements, LogicFormulaResult & result) override;

  ScAddr getFormula() const override;

private:
  ScMemoryContext * context;
};
