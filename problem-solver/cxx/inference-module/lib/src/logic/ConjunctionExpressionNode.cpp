/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ConjunctionExpressionNode.hpp"

#include "classifier/FormulaClassifier.hpp"

ConjunctionExpressionNode::ConjunctionExpressionNode(
    ScMemoryContext * context,
    utils::ScLogger * logger,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : context(context), logger(logger)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

void ConjunctionExpressionNode::compute(LogicFormulaResult & result) const
{
  result.value = false;
  std::vector<TemplateExpressionNode *> formulasWithoutConstants;
  std::vector<TemplateExpressionNode *> formulasToGenerate;

  for (auto const & operand : operands)
  {
    operand->setArgumentVector(argumentVector);
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!FormulaClassifier::isFormulaWithConst(context, atom->getFormula()))
      {
        logger->Debug("Found formula without constants in conjunction");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (FormulaClassifier::isFormulaToGenerate(context, atom->getFormula()))
      {
        logger->Debug("Found formula to generate in conjunction");
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
      ReplacementsUtils::IntersectReplacements(result.replacements, lastResult.replacements, result.replacements);
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
    LogicFormulaResult lastResult = atom->search(result.replacements);
    if (!lastResult.value)
    {
      result.value = false;
      result.isGenerated = false;
      result.replacements = {};
      return;
    }
    ReplacementsUtils::IntersectReplacements(result.replacements, lastResult.replacements, result.replacements);
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
    LogicFormulaResult lastResult;
    formulaToGenerate->generate(result.replacements, lastResult);
    if (!lastResult.value)
    {
      result.value = false;
      result.isGenerated = false;
      result.replacements = {};
      return;
    }
    ReplacementsUtils::IntersectReplacements(result.replacements, lastResult.replacements, result.replacements);
    if (result.replacements.empty())
    {
      result.value = false;
      result.isGenerated = false;
      result.replacements = {};
      return;
    }
  }
}

void ConjunctionExpressionNode::generate(Replacements & replacements, LogicFormulaResult & result)
{
  LogicFormulaResult fail = {false, false, {}};
  result = {true, false, replacements};
  for (auto const & operand : operands)
  {
    LogicFormulaResult lastResult;
    operand->generate(result.replacements, lastResult);
    if (!lastResult.value)
    {
      result = fail;
      return;
    }
    result.isGenerated |= lastResult.isGenerated;
    ReplacementsUtils::IntersectReplacements(result.replacements, lastResult.replacements, result.replacements);
    if (ReplacementsUtils::GetColumnsAmount(result.replacements) == 0)
    {
      result = fail;
      return;
    }
  }
}

ScAddr ConjunctionExpressionNode::getFormula() const
{
  return ScAddr::Empty;
}
