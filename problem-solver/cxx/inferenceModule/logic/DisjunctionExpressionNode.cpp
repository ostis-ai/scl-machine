/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DisjunctionExpressionNode.hpp"

DisjunctionExpressionNode::DisjunctionExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

DisjunctionExpressionNode::DisjunctionExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : DisjunctionExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult DisjunctionExpressionNode::check(ScTemplateParams & params) const
{
  LogicExpressionResult disjunctionResult;
  disjunctionResult.value = false;

  for (auto & operand : operands)
  {
    LogicExpressionResult operandResult = operand->check(params);
    disjunctionResult.formulaTemplate = operandResult.formulaTemplate;

    if (operandResult.hasSearchResult)
      disjunctionResult.replacements = operandResult.replacements;

    if (operandResult.value)
    {
      disjunctionResult.value = true;
      return disjunctionResult;
    }
  }

  return disjunctionResult;
}

LogicFormulaResult DisjunctionExpressionNode::compute(LogicFormulaResult & result) const
{
  LogicFormulaResult fail = {false, false, {}};
  result.value = false;
  vector<TemplateExpressionNode *> formulasWithoutConstants;
  vector<TemplateExpressionNode *> formulasToGenerate;

  for (auto const & operand : operands)
  {
    operand->setArgumentVector(argumentVector);
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!FormulaClassifier::isFormulaWithConst(context, atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula without constants in disjunction");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (FormulaClassifier::isFormulaToGenerate(context, atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula to generate in disjunction");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult lastResult;
    operand->compute(lastResult);
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  if (result.replacements.empty())
    return fail;
  for (auto const & atom : formulasWithoutConstants)
  {
    LogicFormulaResult lastResult = atom->find(result.replacements);
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  for (auto const & formulaToGenerate : formulasToGenerate)
  {
    LogicFormulaResult lastResult = formulaToGenerate->generate(result.replacements);
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  return result;
}
