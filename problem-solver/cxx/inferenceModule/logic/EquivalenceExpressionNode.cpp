/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "EquivalenceExpressionNode.hpp"

EquivalenceExpressionNode::EquivalenceExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : context(context)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

void EquivalenceExpressionNode::compute(LogicFormulaResult & result) const
{
  vector<LogicFormulaResult> subFormulaResults;
  result.value = false;

  vector<TemplateExpressionNode *> formulasWithoutConstants;
  vector<TemplateExpressionNode *> formulasToGenerate;
  for (auto const & operand : operands)
  {
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!FormulaClassifier::isFormulaWithConst(context, atom->getFormula()))
      {
        SC_LOG_DEBUG("Found formula without constants in equivalence");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (FormulaClassifier::isFormulaToGenerate(context, atom->getFormula()))
      {
        SC_LOG_DEBUG("Found formula to generate in equivalence");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult subFormulaResult;
    operand->compute(subFormulaResult);
    subFormulaResults.push_back(subFormulaResult);
  }
  SC_LOG_DEBUG("Processed " << subFormulaResults.size() << " formulas in equivalence");
  if (subFormulaResults.empty())
  {
    SC_LOG_ERROR("All sub formulas in equivalence are either don't have constants or supposed to be generated");
    throw std::exception();
  }

  if (!formulasWithoutConstants.empty())
  {
    SC_LOG_DEBUG("Processing formula without constants");
    auto formulaWithoutConstants = formulasWithoutConstants[0];
    subFormulaResults.push_back(formulaWithoutConstants->find(subFormulaResults[0].replacements));
  }

  if (!formulasToGenerate.empty())
  {
    SC_LOG_DEBUG("Processing formula to generate");
    auto formulaToGenerate = formulasToGenerate[0];
    subFormulaResults.push_back(formulaToGenerate->generate(subFormulaResults[0].replacements));
  }
  result.value = subFormulaResults[0].value == subFormulaResults[1].value;
  if (result.value)
    result.replacements =
        ReplacementsUtils::intersectReplacements(subFormulaResults[0].replacements, subFormulaResults[1].replacements);
  return;

  auto leftAtom = dynamic_cast<TemplateExpressionNode *>(operands[0].get());
  bool isLeftGenerated = (leftAtom) && FormulaClassifier::isFormulaToGenerate(context, leftAtom->getFormula());

  auto rightAtom = dynamic_cast<TemplateExpressionNode *>(operands[1].get());
  bool isRightGenerated = (rightAtom) && FormulaClassifier::isFormulaToGenerate(context, rightAtom->getFormula());

  bool leftHasConstants = (leftAtom) && FormulaClassifier::isFormulaWithConst(context, leftAtom->getFormula());
  bool rightHasConstants = (rightAtom) && FormulaClassifier::isFormulaWithConst(context, rightAtom->getFormula());

  SC_LOG_DEBUG("Left has constants = " << leftHasConstants);
  SC_LOG_DEBUG("Right has constants = " << rightHasConstants);

  LogicFormulaResult leftResult;
  LogicFormulaResult rightResult;

  if (!isLeftGenerated)
  {
    SC_LOG_DEBUG("*** Left part of equivalence shouldn't be generated");
    operands[0]->compute(leftResult);
    if (isRightGenerated)
    {
      rightResult = rightAtom->generate(leftResult.replacements);
    }
    else
    {
      operands[1]->compute(rightResult);
    }
  }
  else
  {
    if (isRightGenerated)
    {
      SC_LOG_DEBUG("*** Right part should be generated");
      return;
    }
    else
    {
      SC_LOG_DEBUG("*** Right part shouldn't be generated");
      operands[1]->compute(rightResult);
      leftResult = leftAtom->generate(rightResult.replacements);
    }
  }

  result.value = leftResult.value == rightResult.value;
  if (rightResult.value)
    result.replacements = ReplacementsUtils::intersectReplacements(leftResult.replacements, rightResult.replacements);
}
