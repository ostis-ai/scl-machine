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
    utils::ScLogger * logger,
    std::shared_ptr<TemplateSearcherAbstract> templateSearcher,
    std::shared_ptr<TemplateManagerAbstract> templateManager,
    std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager,
    ScAddr const & outputStructure)
  : context(context)
  , logger(logger)
  , templateSearcher(std::move(templateSearcher))
  , templateManager(std::move(templateManager))
  , solutionTreeManager(std::move(solutionTreeManager))
  , outputStructure(outputStructure)
{
}

std::shared_ptr<LogicExpressionNode> LogicExpression::build(ScAddr const & formula)
{
  int formulaType = FormulaClassifier::typeOfFormula(context, logger, formula);
  switch (formulaType)
  {
  case FormulaClassifier::ATOMIC:
    return buildAtomicFormula(formula);
  case FormulaClassifier::CONJUNCTION:
    return buildConjunctionFormula(formula);
  case FormulaClassifier::DISJUNCTION:
    return buildDisjunctionFormula(formula);
  case FormulaClassifier::NEGATION:
    return buildNegationFormula(formula);
  case FormulaClassifier::IMPLICATION_ARC:
    return buildImplicationArcFormula(formula);
  case FormulaClassifier::IMPLICATION_TUPLE:
    return buildImplicationTupleFormula(formula);
  case FormulaClassifier::EQUIVALENCE_EDGE:
    return buildEquivalenceEdgeFormula(formula);
  case FormulaClassifier::EQUIVALENCE_TUPLE:
    return buildEquivalenceTupleFormula(formula);
  case FormulaClassifier::NONE:
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Formula is invalid");
  default:
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound, context->GetElementSystemIdentifier(formula) << " is not defined formula type");
  }
}

OperatorLogicExpressionNode::OperandsVector LogicExpression::resolveTupleOperands(ScAddr const & tuple)
{
  ScIterator3Ptr operandsIterator = context->CreateIterator3(tuple, ScType::ConstPermPosArc, ScType::Unknown);

  OperatorLogicExpressionNode::OperandsVector operandsVector;

  while (operandsIterator->Next())
  {
    if (!operandsIterator->Get(2).IsValid())
      continue;
    std::shared_ptr<LogicExpressionNode> op = build(operandsIterator->Get(2));
    operandsVector.emplace_back(std::move(op));
  }
  logger->Debug("Amount of operands in ", context->GetElementSystemIdentifier(tuple), ": ", operandsVector.size());

  return operandsVector;
}

OperatorLogicExpressionNode::OperandsVector LogicExpression::resolveConnectorOperands(ScAddr const & connector)
{
  auto const & [begin, end] = context->GetConnectorIncidentElements(connector);
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

std::shared_ptr<LogicExpressionNode> LogicExpression::buildAtomicFormula(ScAddr const & formula)
{
  logger->Debug(context->GetElementSystemIdentifier(formula), " is atomic logical formula");
  if (!templateManager->GetArguments().empty())
  {
    std::vector<ScTemplateParams> params = templateManager->CreateTemplateParams(formula);
    if (!params.empty() && paramsSet.empty())
    {
      paramsSet = std::move(params);
    }
  }

  return std::make_shared<TemplateExpressionNode>(
      context, logger, templateSearcher, templateManager, solutionTreeManager, outputStructure, formula);
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildConjunctionFormula(ScAddr const & formula)
{
  logger->Debug(context->GetElementSystemIdentifier(formula), " is a conjunction tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveTupleOperands(formula);
  if (!operands.empty())
    return std::make_unique<ConjunctionExpressionNode>(context, logger, operands);
  else
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Conjunction must have operands");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildDisjunctionFormula(ScAddr const & formula)
{
  logger->Debug(context->GetElementSystemIdentifier(formula), " is a disjunction tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveTupleOperands(formula);
  if (!operands.empty())
    return std::make_unique<DisjunctionExpressionNode>(context, logger, operands);
  else
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Disjunction must have operands");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildNegationFormula(ScAddr const & formula)
{
  logger->Debug(context->GetElementSystemIdentifier(formula), " is a negation tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveTupleOperands(formula);
  if (operands.size() == 1)
    return std::make_shared<NegationExpressionNode>(logger, operands[0]);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound, "There is " << operands.size() << " operands in negation, but should be one");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildImplicationArcFormula(ScAddr const & formula)
{
  logger->Debug(context->GetElementSystemIdentifier(formula), " is an implication arc");
  OperatorLogicExpressionNode::OperandsVector operands = resolveConnectorOperands(formula);
  if (operands.size() == 2)
    return std::make_unique<ImplicationExpressionNode>(context, logger, operands);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " << operands.size() << " operands in implication arc, but should be two");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildImplicationTupleFormula(ScAddr const & formula)
{
  logger->Debug(context->GetElementSystemIdentifier(formula), " is an implication tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveOperandsForImplicationTuple(formula);
  if (operands.size() == 2)
    return std::make_unique<ImplicationExpressionNode>(context, logger, operands);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " << operands.size() << " operands in implication tuple, but should be two");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildEquivalenceEdgeFormula(ScAddr const & formula)
{
  logger->Debug(context->GetElementSystemIdentifier(formula), " is an equivalence edge");
  OperatorLogicExpressionNode::OperandsVector operands = resolveConnectorOperands(formula);
  if (operands.size() == 2)
    return std::make_unique<EquivalenceExpressionNode>(context, logger, operands);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " << operands.size() << " operands in equivalence edge, but should be two");
}

std::shared_ptr<LogicExpressionNode> LogicExpression::buildEquivalenceTupleFormula(ScAddr const & formula)
{
  logger->Debug(context->GetElementSystemIdentifier(formula), " is an equivalence tuple");
  OperatorLogicExpressionNode::OperandsVector operands = resolveTupleOperands(formula);
  if (operands.size() == 2)
    return std::make_unique<EquivalenceExpressionNode>(context, logger, operands);
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "There is " << operands.size() << " operands in equivalence tuple, but should be two");
}
