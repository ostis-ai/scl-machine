/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "LogicExpression.hpp"
#include "LogicExpressionNode.hpp"

#include "ConjunctionExpressionNode.hpp"
#include "DisjunctionExpressionNode.hpp"
#include "NegationExpressionNode.hpp"
#include "ImplicationExpressionNode.hpp"
#include "EquivalenceExpressionNode.hpp"
#include "TemplateExpressionNode.hpp"

LogicExpression::LogicExpression(
    ScMemoryContext * context,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddrVector argumentVector)
  : context(context)
  , templateSearcher(templateSearcher)
  , templateManager(templateManager)
  , argumentVector(std::move(argumentVector))
{
}

LogicExpression::LogicExpression(
    ScMemoryContext * context,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddrVector argumentVector,
    ScAddr outputStructure)
  : context(context)
  , templateSearcher(templateSearcher)
  , templateManager(templateManager)
  , argumentVector(std::move(argumentVector))
  , outputStructure(outputStructure)
{
}

std::unique_ptr<LogicExpressionNode> LogicExpression::build(ScAddr const & node)
{
  ScIterator3Ptr atomicFormulaIterator =
      context->Iterator3(InferenceKeynodes::atomic_logical_formula, ScType::EdgeAccessConstPosPerm, node);

  if (atomicFormulaIterator->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a template");
    std::vector<ScTemplateParams> params =
        templateManager->createTemplateParamsList(node, templateSearcher->getParams());

    if (!params.empty() && paramsSet.empty())
    {
      paramsSet = std::move(params);
    }

    return std::make_unique<TemplateExpressionNode>(context, node, templateSearcher, templateManager, outputStructure);
  }

  ScIterator3Ptr conjunctionIterator =
      context->Iterator3(InferenceKeynodes::nrel_conjunction, ScType::EdgeAccessConstPosPerm, node);

  if (conjunctionIterator->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a conjunction tuple");
    auto operands = resolveTupleOperands(node);
    if (!operands.empty())
      return std::make_unique<ConjunctionExpressionNode>(context, operands);
    else
      SC_THROW_EXCEPTION(utils::ScException, "Conjunction must have operands");
  }

  ScIterator3Ptr disjunctionIterator =
      context->Iterator3(InferenceKeynodes::nrel_disjunction, ScType::EdgeAccessConstPosPerm, node);

  if (disjunctionIterator->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a disjunction tuple");
    auto operands = resolveTupleOperands(node);
    if (!operands.empty())
      return std::make_unique<DisjunctionExpressionNode>(context, operands);
    else
      SC_THROW_EXCEPTION(utils::ScException, "Disjunction must have operands");
  }

  ScIterator3Ptr negationIterator =
      context->Iterator3(InferenceKeynodes::nrel_negation, ScType::EdgeAccessConstPosPerm, node);

  if (negationIterator->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a negation tuple");
    auto operands = resolveTupleOperands(node);
    if (operands.size() == 1)
      return std::make_unique<NegationExpressionNode>(context, std::move(operands[0]));
    else
      SC_THROW_EXCEPTION(
          utils::ScException, "There is " + to_string(operands.size()) + " operands in negation, but should be one");
  }

  ScIterator3Ptr implicationIterator =
      context->Iterator3(InferenceKeynodes::nrel_implication, ScType::EdgeAccessConstPosPerm, node);

  if (implicationIterator->Next())
  {
    if (utils::CommonUtils::checkType(context, node, ScType::EdgeDCommon))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an implication edge");
      auto operands = resolveEdgeOperands(node);
      if (operands.size() == 2)
        return std::make_unique<ImplicationExpressionNode>(context, operands);
      else
        SC_THROW_EXCEPTION(
            utils::ScException,
            "There is " + to_string(operands.size()) + " operands in implication edge, but should be two");
    }
    if (utils::CommonUtils::checkType(context, node, ScType::NodeTuple))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an implication tuple");
      auto operands = resolveOperandsForImplicationTuple(node);
      if (operands.size() == 2)
        return std::make_unique<ImplicationExpressionNode>(context, operands);
      else
        SC_THROW_EXCEPTION(
            utils::ScException,
            "There is " + to_string(operands.size()) + " operands in implication tuple, but should be two");
    }
  }
  ScIterator3Ptr equivalenceIterator =
      context->Iterator3(InferenceKeynodes::nrel_equivalence, ScType::EdgeAccessConstPosPerm, node);

  if (equivalenceIterator->Next())
  {
    if (utils::CommonUtils::checkType(context, node, ScType::EdgeUCommon))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence edge");
      auto operands = resolveEdgeOperands(node);
      if (operands.size() == 2)
        return std::make_unique<EquivalenceExpressionNode>(context, operands);
      else
        SC_THROW_EXCEPTION(
            utils::ScException,
            "There is " + to_string(operands.size()) + " operands in equivalence edge, but should be two");
    }
    if (utils::CommonUtils::checkType(context, node, ScType::NodeTuple))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence tuple");
      auto operands = resolveTupleOperands(node);
      if (operands.size() == 2)
        return std::make_unique<EquivalenceExpressionNode>(context, operands);
      else
        SC_THROW_EXCEPTION(
            utils::ScException,
            "There is " + to_string(operands.size()) + " operands in equivalence tuple, but should be two");
    }
  }
  SC_THROW_EXCEPTION(utils::ScException, context->HelperGetSystemIdtf(node) + " is not defined tuple");
}

OperatorLogicExpressionNode::OperandsVector LogicExpression::resolveTupleOperands(ScAddr const & tuple)
{
  ScIterator3Ptr operandsIterator = context->Iterator3(tuple, ScType::EdgeAccessConstPosPerm, ScType::Unknown);

  OperatorLogicExpressionNode::OperandsVector operandsVector;

  while (operandsIterator->Next())
  {
    if (!operandsIterator->Get(2).IsValid())
      continue;
    std::unique_ptr<LogicExpressionNode> op = build(operandsIterator->Get(2));
    operandsVector.emplace_back(std::move(op));
  }
  SC_LOG_DEBUG(
      "[Amount of operands in " + context->HelperGetSystemIdtf(tuple) + "]: " + to_string(operandsVector.size()));

  return operandsVector;
}

OperatorLogicExpressionNode::OperandsVector LogicExpression::resolveEdgeOperands(ScAddr const & edge)
{
  ScAddr begin;
  ScAddr end;
  context->GetEdgeInfo(edge, begin, end);
  OperatorLogicExpressionNode::OperandsVector operandsVector;

  if (begin.IsValid())
  {
    std::unique_ptr<LogicExpressionNode> beginOperand = build(begin);
    operandsVector.emplace_back(std::move(beginOperand));
  }
  if (end.IsValid())
  {
    std::unique_ptr<LogicExpressionNode> endOperand = build(end);
    operandsVector.emplace_back(std::move(endOperand));
  }
  return operandsVector;
}

OperatorLogicExpressionNode::OperandsVector LogicExpression::resolveOperandsForImplicationTuple(ScAddr const & tuple)
{
  ScAddr begin = utils::IteratorUtils::getAnyByOutRelation(context, tuple, InferenceKeynodes::rrel_if);
  ScAddr end = utils::IteratorUtils::getAnyByOutRelation(context, tuple, InferenceKeynodes::rrel_then);

  OperatorLogicExpressionNode::OperandsVector operandsVector;
  if (begin.IsValid())
  {
    std::unique_ptr<LogicExpressionNode> premiseOperand = build(begin);
    operandsVector.emplace_back(std::move(premiseOperand));
  }
  if (end.IsValid())
  {
    std::unique_ptr<LogicExpressionNode> conclusionOperand = build(end);
    operandsVector.emplace_back(std::move(conclusionOperand));
  }
  return operandsVector;
}
