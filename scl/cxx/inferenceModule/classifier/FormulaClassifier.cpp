#include "FormulaClassifier.hpp"

#include <sc-agents-common/utils/CommonUtils.hpp>

namespace inference
{
int FormulaClassifier::NONE = 0;
int FormulaClassifier::ATOM = 1;
int FormulaClassifier::NEGATION = 2;
int FormulaClassifier::CONJUNCTION = 3;
int FormulaClassifier::DISJUNCTION = 4;
int FormulaClassifier::IMPLICATION_EDGE = 5;
int FormulaClassifier::IMPLICATION_TUPLE = 6;
int FormulaClassifier::EQUIVALENCE_EDGE = 7;
int FormulaClassifier::EQUIVALENCE_TUPLE = 8;

FormulaClassifier::FormulaClassifier(ScMemoryContext * ms_context)
  : ms_context(ms_context)
{
}

/*  This method is never used except for tests      */
int FormulaClassifier::typeOfFormula(ScAddr const & formula)
{
  SC_LOG_DEBUG("Checking type of formula " + ms_context->HelperGetSystemIdtf(formula));
  if (!formula.IsValid())
  {
    SC_LOG_DEBUG("Formula is not valid");
    return NONE;
  }

  bool isAtomicFormula =
      ms_context->HelperCheckEdge(InferenceKeynodes::atomic_logical_formula, formula, ScType::EdgeAccessConstPosPerm);
  if (isAtomicFormula)
    return ATOM;
  SC_LOG_DEBUG("Formula is not atom");

  bool isNegation =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_negation, formula, ScType::EdgeAccessConstPosPerm);
  if (isNegation)
    return NEGATION;
  SC_LOG_DEBUG("Formula is not negation");

  bool isConjunction =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_conjunction, formula, ScType::EdgeAccessConstPosPerm);
  if (isConjunction)
    return CONJUNCTION;
  SC_LOG_DEBUG("Formula is not conjunction");

  bool isDisjunction =
      ms_context->HelperCheckEdge(InferenceKeynodes::nrel_disjunction, formula, ScType::EdgeAccessConstPosPerm);
  if (isDisjunction)
    return DISJUNCTION;
  SC_LOG_DEBUG("Formula is not disjunction");

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
  SC_LOG_DEBUG("Formula is not implication");

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
  SC_LOG_DEBUG("Formula is not equivalence");
  return NONE;
}

bool FormulaClassifier::isFormulaWithConst(ScAddr const & formula)
{
  ScIterator3Ptr constNodesIterator = ms_context->Iterator3(formula, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  if (constNodesIterator->Next())
    return true;
  ScIterator3Ptr constLinksIterator = ms_context->Iterator3(formula, ScType::EdgeAccessConstPosPerm, ScType::LinkConst);
  return constLinksIterator->Next();
}

bool FormulaClassifier::isFormulaToGenerate(ScAddr const & formula)
{
  return ms_context->HelperCheckEdge(
      InferenceKeynodes::concept_template_for_generation, formula, ScType::EdgeAccessConstPosPerm);
}

}  // namespace inference
