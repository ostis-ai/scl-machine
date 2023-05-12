/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "NegationExpressionNode.hpp"

NegationExpressionNode::NegationExpressionNode(std::shared_ptr<LogicExpressionNode> op)
{
  operands.emplace_back(std::move(op));
}

NegationExpressionNode::NegationExpressionNode(ScMemoryContext * context, std::shared_ptr<LogicExpressionNode> op)
  : NegationExpressionNode(std::move(op))
{
  this->context = context;
}

LogicFormulaResult NegationExpressionNode::compute(LogicFormulaResult & result) const
{
  LogicFormulaResult const & formulaResult = operands[0]->compute(result);
  std::string formulaValue = (formulaResult.value ? "true" : "false");
  SC_LOG_DEBUG("Sub formula in negation returned " + formulaValue);
  return {!formulaResult.value, formulaResult.isGenerated, formulaResult.replacements};
}
