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

  auto premiseAtom = dynamic_cast<TemplateExpressionNode *>(operands[0].get());
  bool isLeftGenerated =
      (premiseAtom) && FormulaClassifier::isFormulaToGenerate(context, premiseAtom->getFormulaTemplate());

  auto conclusionAtom = dynamic_cast<TemplateExpressionNode *>(operands[1].get());
  bool isRightGenerated =
      (conclusionAtom) && FormulaClassifier::isFormulaToGenerate(context, conclusionAtom->getFormulaTemplate());

  if (!isRightGenerated)
  {
    context->CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::concept_template_for_generation,
        conclusionAtom->getFormulaTemplate());
    isRightGenerated = true;
  }

  LogicFormulaResult premiseResult;
  LogicFormulaResult conclusionResult;

  if (!isLeftGenerated)
  {
    SC_LOG_DEBUG("Premise shouldn't be generated");
    premiseResult = operands[0]->compute(result);
    conclusionResult =
        (isRightGenerated ? conclusionAtom->generate(premiseResult.replacements) : operands[1]->compute(result));
  }
  else
  {
    if (isRightGenerated)
    {
      SC_LOG_DEBUG("Conclusion should be generated");
      return {true, true, {}};
    }
    else
    {
      SC_LOG_DEBUG("Conclusion shouldn't be generated");
      conclusionResult = operands[1]->compute(result);
      premiseResult = premiseAtom->generate(conclusionResult.replacements);
    }
  }

  result.value = !premiseResult.value || conclusionResult.value;
  result.isGenerated = premiseResult.isGenerated || conclusionResult.isGenerated;
  if (conclusionResult.value)
    result.replacements =
        ReplacementsUtils::uniteReplacements(premiseResult.replacements, conclusionResult.replacements);
  return result;
}
