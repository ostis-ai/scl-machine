/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "TemplateExpressionNode.hpp"

using namespace inference;

class NegationExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit NegationExpressionNode(std::shared_ptr<LogicExpressionNode> operand);

  void compute(LogicFormulaResult & result) const override;

  void generate(Replacements & replacements, LogicFormulaResult & result) override;

  ScAddr getFormula() const override;
};
