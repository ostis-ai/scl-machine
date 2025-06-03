/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "EquivalenceExpressionNode.hpp"

#include "classifier/FormulaClassifier.hpp"

EquivalenceExpressionNode::EquivalenceExpressionNode(
    ScMemoryContext * context,
    utils::ScLogger * logger,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : context(context), logger(logger)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

void EquivalenceExpressionNode::compute(LogicFormulaResult & result) const
{
  std::vector<LogicFormulaResult> subFormulaResults;
  result.value = false;

  std::vector<TemplateExpressionNode *> formulasWithoutConstants;
  std::vector<TemplateExpressionNode *> formulasToGenerate;
  for (auto const & operand : operands)
  {
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!FormulaClassifier::isFormulaWithConst(context, atom->getFormula()))
      {
        logger->Debug("Found formula without constants in equivalence");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (FormulaClassifier::isFormulaToGenerate(context, atom->getFormula()))
      {
        logger->Debug("Found formula to generate in equivalence");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult subFormulaResult;
    operand->compute(subFormulaResult);
    subFormulaResults.push_back(subFormulaResult);
  }
  logger->Debug("Processed ", subFormulaResults.size(), " formulas in equivalence");
  if (subFormulaResults.empty())
  {
    logger->Error("All sub formulas in equivalence are either don't have constants or supposed to be generated");
    throw std::exception();
  }

  if (!formulasWithoutConstants.empty())
  {
    logger->Debug("Processing formula without constants");
    auto formulaWithoutConstants = formulasWithoutConstants[0];
    subFormulaResults.push_back(formulaWithoutConstants->search(subFormulaResults[0].replacements));
  }

  if (!formulasToGenerate.empty())
  {
    logger->Debug("Processing formula to generate");
    auto formulaToGenerate = formulasToGenerate[0];
    LogicFormulaResult generationResult;
    formulaToGenerate->generate(subFormulaResults[0].replacements, generationResult);
    subFormulaResults.push_back(generationResult);
  }
  result.value = subFormulaResults[0].value == subFormulaResults[1].value;
  if (result.value)
    ReplacementsUtils::IntersectReplacements(
        subFormulaResults[0].replacements, subFormulaResults[1].replacements, result.replacements);
  return;

  auto leftAtom = dynamic_cast<TemplateExpressionNode *>(operands[0].get());
  bool isLeftGenerated = (leftAtom) && FormulaClassifier::isFormulaToGenerate(context, leftAtom->getFormula());

  auto rightAtom = dynamic_cast<TemplateExpressionNode *>(operands[1].get());
  bool isRightGenerated = (rightAtom) && FormulaClassifier::isFormulaToGenerate(context, rightAtom->getFormula());

  bool leftHasConstants = (leftAtom) && FormulaClassifier::isFormulaWithConst(context, leftAtom->getFormula());
  bool rightHasConstants = (rightAtom) && FormulaClassifier::isFormulaWithConst(context, rightAtom->getFormula());

  logger->Debug("Left has constants = ", leftHasConstants);
  logger->Debug("Right has constants = ", rightHasConstants);

  LogicFormulaResult leftResult;
  LogicFormulaResult rightResult;

  if (!isLeftGenerated)
  {
    logger->Debug("*** Left part of equivalence shouldn't be generated");
    operands[0]->compute(leftResult);
    if (isRightGenerated)
    {
      rightAtom->generate(leftResult.replacements, rightResult);
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
      logger->Debug("*** Right part should be generated");
      return;
    }
    else
    {
      logger->Debug("*** Right part shouldn't be generated");
      operands[1]->compute(rightResult);
      leftAtom->generate(rightResult.replacements, leftResult);
    }
  }

  result.value = leftResult.value == rightResult.value;
  if (rightResult.value)
    ReplacementsUtils::IntersectReplacements(leftResult.replacements, rightResult.replacements, result.replacements);
}

void EquivalenceExpressionNode::generate(Replacements & replacements, LogicFormulaResult & result)
{
  result = {false, false, {}};
}

ScAddr EquivalenceExpressionNode::getFormula() const
{
  return ScAddr::Empty;
}
