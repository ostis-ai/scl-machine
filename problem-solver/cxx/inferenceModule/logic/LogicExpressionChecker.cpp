/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "LogicExpressionChecker.hpp"

std::string OperatorLogicExpressionNode::toString(std::string operatorName) const
{
  std::string result;
  for (const auto & operand : operands)
  {
    result += std::string(", " + operand->toString());
  }

  return std::move(operatorName) + "(" + (result.c_str() + 2) + ")";
}

void OperatorLogicExpressionNode::checkOperands(
    bool & areThereTrue, bool & areThereFalse,
    ScAddr& templateItself,
    ScTemplateSearchResultItem& searchResult,
    bool & isThereSearchResult,
    const ScTemplateParams& params) const
{
  areThereTrue = false;
  areThereFalse = false;

  for (auto & operand : operands)
  {
    auto isOperandTrue = operand->check(params);
    (isOperandTrue.result ? areThereTrue : areThereFalse) = true;
    templateItself = isOperandTrue.templateItself;

    if (isOperandTrue.hasTemplateSearchResult)
    {
      searchResult = isOperandTrue.templateSearchResult;
      isThereSearchResult = true;
    }
  }
}

AndExpressionNode::AndExpressionNode(OperandsVectorType & operands)
{
  for (auto & operand : operands)
  {
    this->operands.emplace_back(std::move(operand));
  }
}

LogicExpressionResult AndExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult res;
  bool areThereTrue, areThereFalse;

  checkOperands(areThereTrue, areThereFalse, res.templateItself,
      res.templateSearchResult, res.hasTemplateSearchResult, params);
  res.result = !areThereFalse;

  return res;
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
  LogicExpressionResult res;
  bool areThereTrue, areThereFalse;

  checkOperands(areThereTrue, areThereFalse, res.templateItself,
                res.templateSearchResult, res.hasTemplateSearchResult, params);
  res.result = areThereTrue;

  return res;
}

NotExpressionNode::NotExpressionNode(std::unique_ptr<LogicExpressionNode> op)
    : op(std::move(op)) {}

LogicExpressionResult NotExpressionNode::check(ScTemplateParams params) const
{
  auto isOperandTrue = op->check(params);

  isOperandTrue.result = !isOperandTrue.result;

  return isOperandTrue;
}

TemplateExpressionNode::TemplateExpressionNode(ScAddr templateToCheck,
                                               inference::TemplateSearcher * templateSearcher,
                                               ScMemoryContext * context)
    : templateToCheck(templateToCheck),
      templateSearcher(templateSearcher),
      context(context) {}

LogicExpressionResult TemplateExpressionNode::check(ScTemplateParams params) const
{
  std::vector<ScTemplateSearchResultItem> searchResult =
      templateSearcher->searchTemplate(templateToCheck, params);

  SC_LOG_DEBUG(context->HelperGetSystemIdtf(templateToCheck) + ": "
              + (!searchResult.empty() ? "true" : "false"))

  if (!searchResult.empty())
  {
    return {true,
            true,
            searchResult[0],
            templateToCheck};
  }
  else
  {
    return {false,
            false,
            {nullptr, nullptr},
            templateToCheck};
  }
}

LogicExpression::LogicExpression(ScAddr condition, ScMemoryContext * context,
                                 inference::TemplateSearcher * templateSearcher,
                                 inference::TemplateManager * templateManager,
                                 std::vector<ScAddr> argumentList)
    : conditionRoot(condition), context(context),
      templateSearcher(templateSearcher), templateManager(templateManager),
      argumentList(std::move(argumentList))
{
  atomicLogicalFormula = context->HelperResolveSystemIdtf("atomic_logical_formula");
  nrelDisjunction = context->HelperResolveSystemIdtf("nrel_disjunction");
  nrelConjunction = context->HelperResolveSystemIdtf("nrel_conjunction");
  nrelNegation = context->HelperResolveSystemIdtf("nrel_negation");
}

void LogicExpression::build()
{
  //collect all the templates within if condition
  //generate for them sets of ScTemplateParams
  //choose the set when IsEmpty() is false, memorize the template the set was generated for

  root = build(conditionRoot);
}

std::unique_ptr<LogicExpressionNode> LogicExpression::build(ScAddr node)
{
  ScIterator3Ptr atomicFormula = context->Iterator3(
      atomicLogicalFormula,
      ScType::EdgeAccessConstPosPerm,
      node);

  ScIterator3Ptr conjunction = context->Iterator3(
      nrelConjunction,
      ScType::EdgeAccessConstPosPerm,
      node);

  ScIterator3Ptr disjunction = context->Iterator3(
      nrelDisjunction,
      ScType::EdgeAccessConstPosPerm,
      node);

  ScIterator3Ptr negation = context->Iterator3(
      nrelNegation,
      ScType::EdgeAccessConstPosPerm,
      node);

  auto findOperands = [this, &node]()
  {
    ScIterator3Ptr operands = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::Unknown);

    OperatorLogicExpressionNode::OperandsVectorType operandsVector;

    while (operands->Next())
    {
      std::unique_ptr<LogicExpressionNode> op =
          build(operands->Get(2));
      operandsVector.emplace_back(std::move(op));
    }

    return operandsVector;
  };

  if (atomicFormula->Next())
  {
    //atomic formula, make TemplateExpressionNode
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a template")
    std::vector<ScTemplateParams> params
        = templateManager->createTemplateParamsList(node, argumentList);

    if (!params.empty() && paramsSet.empty())
    {
      paramsSet = std::move(params);
    }

    return std::make_unique<TemplateExpressionNode>(node, templateSearcher, context);
  }
  else if (conjunction->Next())
  {
    //and, make AndExpressionNode
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a conjunction")

    auto operands = findOperands();

    return std::make_unique<AndExpressionNode>(operands);
  }
  else if (disjunction->Next())
  {
    //or, make OrExpressionNode
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a disjunction")

    auto operands = findOperands();

    return std::make_unique<OrExpressionNode>(operands);
  }
  else if (negation->Next())
  {
    //not, make NotExpressionNode
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a not")

    auto operands = findOperands();

    if (operands.empty())
      throw std::runtime_error("[INFERENCE_MODULE] Not operator doesn't have any arguments");

    return std::make_unique<NotExpressionNode>(std::move(operands[0]));
  }

  throw std::runtime_error("[INFERENCE_MODULE] Unrecognized element of condition tree");
}
