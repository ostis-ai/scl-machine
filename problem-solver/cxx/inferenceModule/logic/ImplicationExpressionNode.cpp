/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ImplicationExpressionNode.hpp"

ImplicationExpressionNode::ImplicationExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

ImplicationExpressionNode::ImplicationExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : ImplicationExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult ImplicationExpressionNode::check(ScTemplateParams & params) const
{
  if (operands.size() != 2)
  {
    SC_LOG_ERROR("Implication should have 2 operands but it has " + to_string(operands.size()));
    return {false, false, {nullptr, nullptr}, ScAddr()};
  }
  LogicExpressionResult premiseResult = operands[0]->check(params);
  LogicExpressionResult conclusionResult = operands[1]->check(params);
}

LogicFormulaResult ImplicationExpressionNode::compute(LogicFormulaResult & result) const
{
  result.value = false;

  LogicExpressionNode * premiseAtom = operands[0].get();
  premiseAtom->setArgumentVector(argumentVector);

  LogicExpressionNode * conclusionAtom = operands[1].get();
  conclusionAtom->setArgumentVector(argumentVector);

  LogicFormulaResult premiseResult = operands[0]->compute(result);
  LogicFormulaResult conclusionResult = conclusionAtom->generate(premiseResult.replacements);

  result.value = !premiseResult.value || conclusionResult.value;
  result.isGenerated = conclusionResult.isGenerated;
  if (conclusionResult.value)
  {
    result.replacements =
          ReplacementsUtils::uniteReplacements(premiseResult.replacements, conclusionResult.replacements);
  }

  return result;
}
