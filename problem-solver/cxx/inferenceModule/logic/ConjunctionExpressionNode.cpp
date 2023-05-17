/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ConjunctionExpressionNode.hpp"

ConjunctionExpressionNode::ConjunctionExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : context(context)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

void ConjunctionExpressionNode::compute(LogicFormulaResult & result) const
{
  result.value = false;
  vector<TemplateExpressionNode *> formulasWithoutConstants;
  vector<TemplateExpressionNode *> formulasToGenerate;

  for (auto const & operand : operands)
  {
    operand->setArgumentVector(argumentVector);
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!FormulaClassifier::isFormulaWithConst(context, atom->getFormula()))
      {
        SC_LOG_DEBUG("Found formula without constants in conjunction");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (FormulaClassifier::isFormulaToGenerate(context, atom->getFormula()))
      {
        SC_LOG_DEBUG("Found formula to generate in conjunction");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult lastResult;
    operand->compute(lastResult);
    if (!lastResult.value)
    {
      result.value = false;
      result.isGenerated = false;
      result.replacements = {};
      return;
    }
    if (!result.value)  // this is true only when processing the first operand
      result = lastResult;
    else
    {
      result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
      if (result.replacements.empty())
      {
        result.value = false;
        result.isGenerated = false;
        result.replacements = {};
        return;
      }
    }
  }
  for (auto const & atom : formulasWithoutConstants)  // atoms without constants are processed here
  {
    LogicFormulaResult lastResult = atom->find(result.replacements);
    if (!lastResult.value)
    {
      result.value = false;
      result.isGenerated = false;
      result.replacements = {};
      return;
    }
    result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
    if (result.replacements.empty())
    {
      result.value = false;
      result.isGenerated = false;
      result.replacements = {};
      return;
    }
  }
  for (auto const & formulaToGenerate : formulasToGenerate)  // atoms which should be generated are processed here
  {
    LogicFormulaResult lastResult = formulaToGenerate->generate(result.replacements);
    if (!lastResult.value)
    {
      result.value = false;
      result.isGenerated = false;
      result.replacements = {};
      return;
    }
    result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
    if (result.replacements.empty())
    {
      result.value = false;
      result.isGenerated = false;
      result.replacements = {};
      return;
    }
  }
}

LogicFormulaResult ConjunctionExpressionNode::generate(Replacements & replacements)
{
  LogicFormulaResult fail = {false, false, {}};
  LogicFormulaResult globalResult = {true, false, replacements};
  for (auto const & operand : operands)
  {
    LogicFormulaResult lastResult = operand->generate(globalResult.replacements);
    if (!lastResult.value)
      return fail;
    globalResult.isGenerated |= lastResult.isGenerated;
    globalResult.replacements =
        ReplacementsUtils::intersectReplacements(globalResult.replacements, lastResult.replacements);
    if (ReplacementsUtils::getColumnsAmount(globalResult.replacements) == 0)
      return fail;
  }
  return globalResult;
}
