/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "FormulaClassifier.hpp"

#include <sc-agents-common/utils/CommonUtils.hpp>

namespace inference
{
int FormulaClassifier::typeOfFormula(ScMemoryContext * ms_context, utils::ScLogger * logger, ScAddr const & formula)
{
  if (!formula.IsValid())
  {
    logger->Error("Formula is not valid");
    return NONE;
  }

  // TODO(MksmOrlov): implement the agent of logical formulas verification, check types and number of operands
  bool isAtomicFormula =
      ms_context->CheckConnector(InferenceKeynodes::atomic_logical_formula, formula, ScType::ConstPermPosArc);
  if (!isAtomicFormula)
  {
    if (ms_context->GetElementType(formula) == ScType::ConstNodeStructure && isFormulaWithVar(ms_context, formula))
    {
      isAtomicFormula = true;
    }
  }
  if (isAtomicFormula)
    return ATOMIC;

  bool const isImplication =
      ms_context->CheckConnector(InferenceKeynodes::nrel_implication, formula, ScType::ConstPermPosArc);
  if (isImplication)
  {
    if (ms_context->GetElementType(formula) == ScType::ConstCommonArc)
      return IMPLICATION_ARC;
    if (ms_context->GetElementType(formula) == ScType::ConstNodeTuple)
      return IMPLICATION_TUPLE;
    return NONE;
  }

  bool const isNegation =
      ms_context->CheckConnector(InferenceKeynodes::nrel_negation, formula, ScType::ConstPermPosArc);
  if (isNegation)
    return NEGATION;

  bool const isConjunction =
      ms_context->CheckConnector(InferenceKeynodes::nrel_conjunction, formula, ScType::ConstPermPosArc);
  if (isConjunction)
    return CONJUNCTION;

  bool const isDisjunction =
      ms_context->CheckConnector(InferenceKeynodes::nrel_disjunction, formula, ScType::ConstPermPosArc);
  if (isDisjunction)
    return DISJUNCTION;

  bool const isEquivalence =
      ms_context->CheckConnector(InferenceKeynodes::nrel_equivalence, formula, ScType::ConstPermPosArc);
  if (isEquivalence)
  {
    if (ms_context->GetElementType(formula) == ScType::ConstCommonEdge)
      return EQUIVALENCE_EDGE;
    if (ms_context->GetElementType(formula) == ScType::ConstNodeTuple)
      return EQUIVALENCE_TUPLE;
  }

  return NONE;
}

bool FormulaClassifier::isFormulaWithConst(ScMemoryContext * ms_context, ScAddr const & formula)
{
  ScIterator3Ptr constNodesIterator = ms_context->CreateIterator3(formula, ScType::ConstPermPosArc, ScType::ConstNode);
  if (constNodesIterator->Next())
    return true;
  ScIterator3Ptr constLinksIterator =
      ms_context->CreateIterator3(formula, ScType::ConstPermPosArc, ScType::ConstNodeLink);
  return constLinksIterator->Next();
}

bool FormulaClassifier::isFormulaWithVar(ScMemoryContext * ms_context, ScAddr const & formula)
{
  ScIterator3Ptr varNodesIterator = ms_context->CreateIterator3(formula, ScType::ConstPermPosArc, ScType::VarNode);
  if (varNodesIterator->Next())
    return true;
  ScIterator3Ptr varLinksIterator = ms_context->CreateIterator3(formula, ScType::ConstPermPosArc, ScType::VarNodeLink);
  return varLinksIterator->Next();
}

bool FormulaClassifier::isFormulaToGenerate(ScMemoryContext * ms_context, ScAddr const & formula)
{
  return ms_context->CheckConnector(
      InferenceKeynodes::concept_template_for_generation, formula, ScType::ConstPermPosArc);
}

}  // namespace inference
