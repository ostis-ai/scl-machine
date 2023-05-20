/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ImplicationExpressionNode.hpp"

ImplicationExpressionNode::ImplicationExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : context(context)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

/**
 * @brief Search premise and get it's replacements. Generate conclusion with found replacements
 * @param result is a LogicFormulaResult{bool: value, value: isGenerated, Replacements: replacements}
 * @return result from param
 */
void ImplicationExpressionNode::compute(LogicFormulaResult & result) const
{
  LogicExpressionNode * premiseAtom = operands[0].get();
  premiseAtom->setArgumentVector(argumentVector);

  LogicExpressionNode * conclusionAtom = operands[1].get();
  conclusionAtom->setArgumentVector(argumentVector);

  // Compute premise formula, get replacements with found constructions
  LogicFormulaResult premiseResult;
  premiseAtom->compute(premiseResult);

  // Generate conclusion using computed premise replacements
  LogicFormulaResult conclusionResult = conclusionAtom->generate(premiseResult.replacements);

  // Implication value (a -> b) is equal to ((!a) || b)
  result.value = !premiseResult.value || conclusionResult.value;
  result.isGenerated = conclusionResult.isGenerated;
  if (conclusionResult.value)
  {
    result.replacements =
        ReplacementsUtils::intersectReplacements(premiseResult.replacements, conclusionResult.replacements);
  }
}
