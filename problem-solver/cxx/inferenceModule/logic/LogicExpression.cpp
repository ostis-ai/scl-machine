/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "LogicExpression.hpp"

AndExpressionNode::AndExpressionNode(OperandsVectorType & operands)
{
  for (auto & operand : operands)
  {
    this->operands.emplace_back(std::move(operand));
  }
}

LogicExpressionResult AndExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult operatorResult;

  for (auto & operand : operands)
  {
    auto checkResult = operand->check(params);
    operatorResult.formulaTemplate = checkResult.formulaTemplate;

    if (checkResult.hasSearchResult)
    {
      operatorResult.templateSearchResult = checkResult.templateSearchResult;
    }

    if (!checkResult.value)
    {
      operatorResult.value = false;
      return operatorResult;
    }
  }

  return operatorResult;
}

OrExpressionNode::OrExpressionNode(OperandsVectorType & operands)
{
  for (auto & operand : operands)
  {
    this->operands.emplace_back(std::move(operand));
  }
}

LogicExpressionResult OrExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult operatorResult;

  for (auto & operand : operands)
  {
    auto checkResult = operand->check(params);
    operatorResult.formulaTemplate = checkResult.formulaTemplate;

    if (checkResult.hasSearchResult)
    {
      operatorResult.templateSearchResult = checkResult.templateSearchResult;
    }

    if (checkResult.value)
    {
      operatorResult.value = true;
      return operatorResult;
    }
  }

  return operatorResult;
}

NotExpressionNode::NotExpressionNode(std::unique_ptr<LogicExpressionNode> op)
{
  operands.emplace_back(std::move(op));
}

LogicExpressionResult NotExpressionNode::check(ScTemplateParams params) const
{
  auto checkResult = operands[0]->check(params);
  checkResult.value = !checkResult.value;

  return checkResult;
}

TemplateExpressionNode::TemplateExpressionNode(
    ScMemoryContext * context,
    ScAddr formulaTemplate,
    TemplateSearcher * templateSearcher
)
  : context(context),
    formulaTemplate(formulaTemplate),
    templateSearcher(templateSearcher) {}

LogicExpressionResult TemplateExpressionNode::check(ScTemplateParams params) const
{
  auto searchResult = templateSearcher->searchTemplate(formulaTemplate, params);
  std::string result = (!searchResult.empty() ? "right" : "wrong");
  SC_LOG_DEBUG("Statement " + context->HelperGetSystemIdtf(formulaTemplate) + " " + result);

  if (!searchResult.empty())
  {
    return {
      true,
      true,
      searchResult[0],
      formulaTemplate
    };
  }
  else
  {
    return {
      false,
      false,
      {nullptr, nullptr},
      formulaTemplate
    };
  }
}

LogicExpression::LogicExpression(
    ScMemoryContext * context,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddrVector argumentList
) : context(context),
    templateSearcher(templateSearcher),
    templateManager(templateManager),
    argumentList(std::move(argumentList))
{
}

std::unique_ptr<LogicExpressionNode> LogicExpression::build(ScAddr const & node)
{
  auto resolveOperands = [this](ScAddr const & node)
  {
    ScIterator3Ptr operands = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::Unknown);

    OperatorLogicExpressionNode::OperandsVectorType operandsVector;

    while (operands->Next())
    {
      std::unique_ptr<LogicExpressionNode> op = build(operands->Get(2));
      operandsVector.emplace_back(std::move(op));
    }

    return operandsVector;
  };

  ScIterator3Ptr atomicFormulaIter3 = context->Iterator3(
      InferenceKeynodes::atomic_logical_formula,
      ScType::EdgeAccessConstPosPerm,
      node
  );

  if (atomicFormulaIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node); + " is a template");
    std::vector<ScTemplateParams> params = templateManager->createTemplateParamsList(node, argumentList);

    if (!params.empty() && paramsSet.empty())
    {
      paramsSet = std::move(params);
    }

    return std::make_unique<TemplateExpressionNode>(context, node, templateSearcher);
  }

  ScIterator3Ptr conjunctionIter3 = context->Iterator3(
      InferenceKeynodes::nrel_conjunction,
      ScType::EdgeAccessConstPosPerm,
      node
  );

  if (conjunctionIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node); + " is a conjunction tuple");
    auto operands = resolveOperands(node);

    return std::make_unique<AndExpressionNode>(operands);
  }

  ScIterator3Ptr disjunctionIter3 = context->Iterator3(
      InferenceKeynodes::nrel_disjunction,
      ScType::EdgeAccessConstPosPerm,
      node
  );

  if (disjunctionIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node); + " is a disjunction tuple");
    auto operands = resolveOperands(node);

    return std::make_unique<OrExpressionNode>(operands);
  }

  ScIterator3Ptr negationIter3 = context->Iterator3(
      InferenceKeynodes::nrel_negation,
      ScType::EdgeAccessConstPosPerm,
      node
  );

  if (negationIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node); + " is a negation tuple");
    auto operands = resolveOperands(node);

    return std::make_unique<NotExpressionNode>(std::move(operands[0]));
  }

  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node); + " is not defined tuple");
  return {};
}
