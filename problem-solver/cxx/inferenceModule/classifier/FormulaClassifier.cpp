/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "FormulaClassifier.hpp"

#include <sc-agents-common/utils/CommonUtils.hpp>

namespace inference
{
int FormulaClassifier::typeOfFormula(ScMemoryContext * ms_context, ScAddr const & formula)
{
  if (!formula.IsValid())
  {
    SC_LOG_ERROR("Formula is not valid");
    return NONE;
  }

  // TODO(MksmOrlov): implement the agent of logical formulas verification, check types and number of operands
  bool isAtomicFormula =
      ms_context->HelperCheckEdge(InferenceKeynodes::atomic_logical_formula, formula, ScType::EdgeAccessConstPosPerm);
  if (!isAtomicFormula)
  {
    if (ms_context->GetElementType(formula) == ScType::NodeConstStruct && isFormulaWithVar(ms_context, formula))
    {
      isAtomicFormula = true;
    }
  }
  if (isAtomicFormula)
    return ATOMIC;

  bool const isImplication =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_implication, formula, ScType::EdgeAccessConstPosPerm);
  if (isImplication)
  {
    if (ms_context->GetElementType(formula) == ScType::EdgeDCommonConst)
      return IMPLICATION_EDGE;
    if (ms_context->GetElementType(formula) == ScType::NodeConstTuple)
      return IMPLICATION_TUPLE;
    return NONE;
  }

  bool const isNegation =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_negation, formula, ScType::EdgeAccessConstPosPerm);
  if (isNegation)
    return NEGATION;

  bool const isConjunction =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_conjunction, formula, ScType::EdgeAccessConstPosPerm);
  if (isConjunction)
    return CONJUNCTION;

  bool const isDisjunction =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_disjunction, formula, ScType::EdgeAccessConstPosPerm);
  if (isDisjunction)
    return DISJUNCTION;

  bool const isEquivalence =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_equivalence, formula, ScType::EdgeAccessConstPosPerm);
  if (isEquivalence)
  {
    if (ms_context->GetElementType(formula) == ScType::EdgeUCommonConst)
      return EQUIVALENCE_EDGE;
    if (ms_context->GetElementType(formula) == ScType::NodeConstTuple)
      return EQUIVALENCE_TUPLE;
  }

  return NONE;
}

bool FormulaClassifier::isFormulaWithConst(ScMemoryContext * ms_context, ScAddr const & formula)
{
  ScIterator3Ptr constNodesIterator = ms_context->Iterator3(formula, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  if (constNodesIterator->Next())
    return true;
  ScIterator3Ptr constLinksIterator = ms_context->Iterator3(formula, ScType::EdgeAccessConstPosPerm, ScType::LinkConst);
  return constLinksIterator->Next();
}

bool FormulaClassifier::isFormulaWithVar(ScMemoryContext * ms_context, ScAddr const & formula)
{
  ScIterator3Ptr varNodesIterator = ms_context->Iterator3(formula, ScType::EdgeAccessConstPosPerm, ScType::NodeVar);
  if (varNodesIterator->Next())
    return true;
  ScIterator3Ptr varLinksIterator = ms_context->Iterator3(formula, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  return varLinksIterator->Next();
}

bool FormulaClassifier::isFormulaToGenerate(ScMemoryContext * ms_context, ScAddr const & formula)
{
  return ms_context->HelperCheckEdge(
      InferenceKeynodes::concept_template_for_generation, formula, ScType::EdgeAccessConstPosPerm);
}

}  // namespace inference
