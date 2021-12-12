/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "LogicExpressionChecker.hpp"

AndExpressionNode::AndExpressionNode(std::unique_ptr<LogicExpressionNode> opA,
                                     std::unique_ptr<LogicExpressionNode> opB)
    : leftOp(std::move(opA)), rightOp(std::move(opB)) {}

LogicExpressionResult AndExpressionNode::check(ScTemplateParams params) const
{
  bool finalResult = false;

  auto isLeftOperandTrue = leftOp->check(params);
  LogicExpressionResult isRightOperandTrue;
  if (isLeftOperandTrue.result)
  {
    isRightOperandTrue = rightOp->check(params);

    if (isRightOperandTrue.result)
      finalResult = true;
  }

  bool isThereSearchResultLeft = isLeftOperandTrue.hasTemplateSearchResult;
  bool isThereSearchResultRight = isRightOperandTrue.hasTemplateSearchResult;

  LogicExpressionResult res;
  res.result = finalResult;
  res.hasTemplateSearchResult = finalResult ? isThereSearchResultLeft || isThereSearchResultRight : false;
  res.templateSearchResult = isThereSearchResultLeft ? isLeftOperandTrue.templateSearchResult :
                             (isThereSearchResultRight ? isRightOperandTrue.templateSearchResult :
                              ScTemplateSearchResultItem(nullptr, nullptr));
  res.templateItself = isLeftOperandTrue.templateItself;

  return res;
}

OrExpressionNode::OrExpressionNode(std::unique_ptr<LogicExpressionNode> opA,
                                   std::unique_ptr<LogicExpressionNode> opB)
    : leftOp(std::move(opA)), rightOp(std::move(opB)) {}

LogicExpressionResult OrExpressionNode::check(ScTemplateParams params) const
{
  bool finalResult = true;

  auto isLeftOperandTrue = leftOp->check(params);
  LogicExpressionResult isRightOperandTrue;
  if (!isLeftOperandTrue.result)
  {
    isRightOperandTrue = rightOp->check(params);

    if (!isRightOperandTrue.result)
      finalResult = false;
  }

  bool isThereSearchResultLeft = isLeftOperandTrue.hasTemplateSearchResult;
  bool isThereSearchResultRight = isRightOperandTrue.hasTemplateSearchResult;

  LogicExpressionResult res;
  res.result = finalResult;
  res.hasTemplateSearchResult = finalResult ? isThereSearchResultLeft || isThereSearchResultRight : false;
  res.templateSearchResult = isThereSearchResultLeft ? isLeftOperandTrue.templateSearchResult :
                             (isThereSearchResultRight ? isRightOperandTrue.templateSearchResult :
                              ScTemplateSearchResultItem(nullptr, nullptr));
  res.templateItself = isLeftOperandTrue.templateItself;

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

  ScIterator3Ptr operands = context->Iterator3(
      node,
      ScType::EdgeAccessConstPosPerm,
      ScType::Unknown);

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
    if (operands->Next())
    {
      std::unique_ptr<LogicExpressionNode> opA =
          build(operands->Get(2));

      if (operands->Next())
      {
        std::unique_ptr<LogicExpressionNode> opB =
            build(operands->Get(2));

        return std::make_unique<AndExpressionNode>(std::move(opA),
                                                   std::move(opB));
      }
    }
  }
  else if (disjunction->Next())
  {
    //or, make OrExpressionNode
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a disjunction")
    if (operands->Next())
    {
      std::unique_ptr<LogicExpressionNode> opA =
          build(operands->Get(2));

      if (operands->Next())
      {
        std::unique_ptr<LogicExpressionNode> opB =
            build(operands->Get(2));

        return std::make_unique<OrExpressionNode>(std::move(opA),
                                                  std::move(opB));
      }
    }
  }
  else if (negation->Next())
  {
    //not, make NotExpressionNode
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a not")
    if (operands->Next())
    {
      std::unique_ptr<LogicExpressionNode> op =
          build(operands->Get(2));

      return std::make_unique<NotExpressionNode>(std::move(op));
    }
  }

  throw runtime_error("[INFERENCE_MODULE] Unrecognized element of condition tree");
}
