/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DisjunctionExpressionNode.hpp"

DisjunctionExpressionNode::DisjunctionExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : context(context)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

void DisjunctionExpressionNode::compute(LogicFormulaResult & result) const
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
        SC_LOG_DEBUG("Found formula without constants in disjunction");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (FormulaClassifier::isFormulaToGenerate(context, atom->getFormula()))
      {
        SC_LOG_DEBUG("Found formula to generate in disjunction");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult lastResult;
    operand->compute(lastResult);
    result.value |= lastResult.value;
    ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements, result.replacements);
  }
  if (result.replacements.empty())
  {
    result.value = false;
    result.isGenerated = false;
    result.replacements = {};
    return;
  }
  for (auto const & atom : formulasWithoutConstants)
  {
    LogicFormulaResult lastResult = atom->search(result.replacements);
    result.value |= lastResult.value;
    ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements, result.replacements);
  }
  for (auto const & formulaToGenerate : formulasToGenerate)
  {
    LogicFormulaResult lastResult;
    formulaToGenerate->generate(result.replacements, lastResult);
    result.value |= lastResult.value;
    ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements, result.replacements);
  }
}

void DisjunctionExpressionNode::generate(Replacements & replacements, LogicFormulaResult & result)
{
  result = {false, false, {}};
}

ScAddr DisjunctionExpressionNode::getFormula() const
{
  return ScAddr::Empty;
}
