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

  // TODO(MksmOrlov): maybe we need just to check element type (struct) with/without variables
  bool isAtomicFormula =
      ms_context->HelperCheckEdge(InferenceKeynodes::atomic_logical_formula, formula, ScType::EdgeAccessConstPosPerm);
  if (isAtomicFormula)
    return ATOM;

  bool isNegation =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_negation, formula, ScType::EdgeAccessConstPosPerm);
  if (isNegation)
    return NEGATION;

  bool isConjunction =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_conjunction, formula, ScType::EdgeAccessConstPosPerm);
  if (isConjunction)
    return CONJUNCTION;

  bool isDisjunction =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_disjunction, formula, ScType::EdgeAccessConstPosPerm);
  if (isDisjunction)
    return DISJUNCTION;

  bool isImplication =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_implication, formula, ScType::EdgeAccessConstPosPerm);
  if (isImplication)
  {
    bool isEdge = (utils::CommonUtils::checkType(ms_context, formula, ScType::EdgeDCommon));
    if (isEdge)
      return IMPLICATION_EDGE;
    bool isTuple = (utils::CommonUtils::checkType(ms_context, formula, ScType::NodeTuple));
    if (isTuple)
      return IMPLICATION_TUPLE;
    return NONE;
  }

  bool isEquivalence =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_equivalence, formula, ScType::EdgeAccessConstPosPerm);
  if (isEquivalence)
  {
    bool isEdge = (utils::CommonUtils::checkType(ms_context, formula, ScType::EdgeUCommonConst));
    if (isEdge)
      return EQUIVALENCE_EDGE;
    bool isTuple = (utils::CommonUtils::checkType(ms_context, formula, ScType::NodeTuple));
    if (isTuple)
      return EQUIVALENCE_TUPLE;
    return NONE;
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

bool FormulaClassifier::isFormulaToGenerate(ScMemoryContext * ms_context, ScAddr const & formula)
{
  return ms_context->HelperCheckEdge(
      InferenceKeynodes::concept_template_for_generation, formula, ScType::EdgeAccessConstPosPerm);
}

}  // namespace inference
