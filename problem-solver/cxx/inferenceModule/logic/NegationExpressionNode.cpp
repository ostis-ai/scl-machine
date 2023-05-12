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

LogicFormulaResult NegationExpressionNode::compute(LogicFormulaResult & result) const
{
  LogicFormulaResult const & formulaResult = operands[0]->compute(result);
  SC_LOG_DEBUG("Sub formula in negation returned " << (formulaResult.value ? "true" : "false"));
  return {!formulaResult.value, formulaResult.isGenerated, formulaResult.replacements};
}
