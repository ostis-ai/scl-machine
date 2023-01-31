/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ConjunctionExpressionNode.hpp"

ConjunctionExpressionNode::ConjunctionExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

ConjunctionExpressionNode::ConjunctionExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : ConjunctionExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult ConjunctionExpressionNode::check(ScTemplateParams & params) const
{
  LogicExpressionResult conjunctionResult;
  conjunctionResult.value = true;

  for (auto & operand : operands)
  {
    LogicExpressionResult operandResult = operand->check(params);
    conjunctionResult.formulaTemplate = operandResult.formulaTemplate;
    if (operandResult.hasSearchResult)
      conjunctionResult.templateSearchResult = operandResult.templateSearchResult;

    if (!operandResult.value)
    {
      conjunctionResult.value = false;
      return conjunctionResult;
    }
  }

  return conjunctionResult;
}

LogicFormulaResult ConjunctionExpressionNode::compute(LogicFormulaResult & result) const
{
  result.value = false;
  LogicFormulaResult fail = {false, false, {}};
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
        SC_LOG_DEBUG("Found formula without constants in conjunction");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (FormulaClassifier::isFormulaToGenerate(context, atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula to generate in conjunction");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult lastResult = operand->compute(result);
    if (!lastResult.value)
      return fail;
    if (!result.value)  // this is true only when processing the first operand
      result = lastResult;
    else
    {
      result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
      if (result.replacements.empty())
        return fail;
    }
  }
  for (auto const & atom : formulasWithoutConstants)  // atoms without constants are processed here
  {
    LogicFormulaResult lastResult = atom->find(result.replacements);
    if (!lastResult.value)
      return fail;
    result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
    if (result.replacements.empty())
      return fail;
  }
  for (auto const & formulaToGenerate : formulasToGenerate)  // atoms which should be generated are processed here
  {
    LogicFormulaResult lastResult = formulaToGenerate->generate(result.replacements);
    if (!lastResult.value)
      return fail;
    result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
    if (result.replacements.empty())
      return fail;
  }
  return result;
}
