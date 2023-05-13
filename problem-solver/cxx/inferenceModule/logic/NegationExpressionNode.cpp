/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "NegationExpressionNode.hpp"

NegationExpressionNode::NegationExpressionNode(std::shared_ptr<LogicExpressionNode> operand)
{
  operands.emplace_back(std::move(operand));
}

void NegationExpressionNode::compute(LogicFormulaResult & result) const
{
  operands[0]->compute(result);
  SC_LOG_DEBUG("Sub formula in negation returned " << (result.value ? "true" : "false"));
  result.value = !result.value;
}
