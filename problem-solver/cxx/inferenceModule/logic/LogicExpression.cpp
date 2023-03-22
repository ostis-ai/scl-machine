/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "LogicExpression.hpp"

#include <utility>
#include "LogicExpressionNode.hpp"

#include "ConjunctionExpressionNode.hpp"
#include "DisjunctionExpressionNode.hpp"
#include "NegationExpressionNode.hpp"
#include "ImplicationExpressionNode.hpp"
#include "EquivalenceExpressionNode.hpp"
#include "TemplateExpressionNode.hpp"

LogicExpression::LogicExpression(
    ScMemoryContext * context,
    std::shared_ptr<TemplateSearcherAbstract> templateSearcher,
    std::shared_ptr<TemplateManager> templateManager,
    std::shared_ptr<SolutionTreeManager> solutionTreeManager,
    ScAddr const & outputStructure,
    ScAddr const & rule)
  : context(context)
  , templateSearcher(std::move(templateSearcher))
  , templateManager(std::move(templateManager))
  , solutionTreeManager(std::move(solutionTreeManager))
  , outputStructure(outputStructure)
  , rule(rule)
{
}

std::shared_ptr<LogicExpressionNode> LogicExpression::build(ScAddr const & node)
{
  int formulaType = FormulaClassifier::typeOfFormula(context, node);
  switch (formulaType)
  {
  case FormulaClassifier::ATOM:
    return buildAtomicFormula(node);
  case FormulaClassifier::CONJUNCTION:
    return buildConjunctionFormula(node);
  case FormulaClassifier::DISJUNCTION:
    return buildDisjunctionFormula(node);
  case FormulaClassifier::NEGATION:
    return buildNegationFormula(node);
  case FormulaClassifier::IMPLICATION_EDGE:
    return buildImplicationEdgeFormula(node);
  case FormulaClassifier::IMPLICATION_TUPLE:
    return buildImplicationTupleFormula(node);
  case FormulaClassifier::EQUIVALENCE_EDGE:
    return buildEquivalenceEdgeFormula(node);
  case FormulaClassifier::EQUIVALENCE_TUPLE:
    return buildEquivalenceTupleFormula(node);
  default:
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, context->HelperGetSystemIdtf(node) + " is not defined tuple");
  }
}

OperatorLogicExpressionNode::OperandsVector LogicExpression::resolveTupleOperands(ScAddr const & tuple)
{
  ScIterator3Ptr operandsIterator = context->Iterator3(tuple, ScType::EdgeAccessConstPosPerm, ScType::Unknown);

  OperatorLogicExpressionNode::OperandsVector operandsVector;

  while (operandsIterator->Next())
  {
    if (!operandsIterator->Get(2).IsValid())
      continue;
    std::shared_ptr<LogicExpressionNode> op = build(operandsIterator->Get(2));
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
    std::shared_ptr<LogicExpressionNode> beginOperand = build(begin);
    operandsVector.emplace_back(std::move(beginOperand));
  }
  if (end.IsValid())
  {
    std::shared_ptr<LogicExpressionNode> endOperand = build(end);
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
    std::shared_ptr<LogicExpressionNode> premiseOperand = build(begin);
    operandsVector.emplace_back(std::move(premiseOperand));
  }
  if (end.IsValid())
  {
    std::shared_ptr<LogicExpressionNode> conclusionOperand = build(end);
    operandsVector.emplace_back(std::move(conclusionOperand));
  }
  return operandsVector;
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildAtomicFormula(ScAddr const & node)
{
  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a template");
  std::vector<ScTemplateParams> params = templateManager->createTemplateParams(node, templateSearcher->getParams());

  if (!params.empty() && paramsSet.empty())
  {
    paramsSet = std::move(params);
  }

  return std::make_shared<TemplateExpressionNode>(context, node, templateSearcher, templateManager, solutionTreeManager, outputStructure, rule);
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildConjunctionFormula(ScAddr const & node)
{
  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a conjunction tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveTupleOperands(node);
  if (!operands.empty())
    return std::make_unique<ConjunctionExpressionNode>(context, operands);
  else
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Conjunction must have operands");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildDisjunctionFormula(ScAddr const & node)
{
  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a disjunction tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveTupleOperands(node);
  if (!operands.empty())
    return std::make_unique<DisjunctionExpressionNode>(context, operands);
  else
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Disjunction must have operands");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildNegationFormula(ScAddr const & node)
{
  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a negation tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveTupleOperands(node);
  if (operands.size() == 1)
    return std::make_shared<NegationExpressionNode>(context, operands[0]);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " + to_string(operands.size()) + " operands in negation, but should be one");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildImplicationEdgeFormula(ScAddr const & node)
{
  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an implication edge");
  OperatorLogicExpressionNode::OperandsVector operands = resolveEdgeOperands(node);
  if (operands.size() == 2)
    return std::make_unique<ImplicationExpressionNode>(context, operands);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " + to_string(operands.size()) + " operands in implication edge, but should be two");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildImplicationTupleFormula(const ScAddr & node)
{
  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an implication tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveOperandsForImplicationTuple(node);
  if (operands.size() == 2)
    return std::make_unique<ImplicationExpressionNode>(context, operands);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " + to_string(operands.size()) + " operands in implication tuple, but should be two");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildEquivalenceEdgeFormula(const ScAddr & node)
{
  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence edge");
  OperatorLogicExpressionNode::OperandsVector operands = resolveEdgeOperands(node);
  if (operands.size() == 2)
    return std::make_unique<EquivalenceExpressionNode>(context, operands);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " + to_string(operands.size()) + " operands in equivalence edge, but should be two");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildEquivalenceTupleFormula(const ScAddr & node)
{
  SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveTupleOperands(node);
  if (operands.size() == 2)
    return std::make_unique<EquivalenceExpressionNode>(context, operands);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " + to_string(operands.size()) + " operands in equivalence tuple, but should be two");
}
