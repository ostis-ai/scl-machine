/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "NegationExpressionNode.hpp"

NegationExpressionNode::NegationExpressionNode(std::unique_ptr<LogicExpressionNode> op)
{
  operands.emplace_back(std::move(op));
}

NegationExpressionNode::NegationExpressionNode(ScMemoryContext * context, std::unique_ptr<LogicExpressionNode> op)
      : NegationExpressionNode(std::move(op))
{
  this->context = context;
}

LogicExpressionResult NegationExpressionNode::check(ScTemplateParams & params) const
{
  if (operands.size() != 1)
  {
    SC_LOG_ERROR("Negation should have 1 operand but it has " + to_string(operands.size()));
    return {false, false, {nullptr, nullptr}, ScAddr()};
  }
  LogicExpressionResult operandResult = operands[0]->check(params);
  operandResult.value = !operandResult.value;

  return operandResult;
}

LogicFormulaResult NegationExpressionNode::compute(LogicFormulaResult & result) const
{
  const LogicFormulaResult & formulaResult = operands[0]->compute(result);
  std::string formulaValue = (formulaResult.value ? "true" : "false");
  SC_LOG_DEBUG("Sub formula in negation returned " + formulaValue);
  return {!formulaResult.value, formulaResult.isGenerated, formulaResult.replacements};
}
