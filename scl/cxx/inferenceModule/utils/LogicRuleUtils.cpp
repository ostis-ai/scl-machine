/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "LogicRuleUtils.hpp"

#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"

using namespace scAgentsCommon;

namespace inference
{
ScAddr LogicRuleUtils::getFormulaPremise(ScMemoryContext * context, ScAddr const & logicalFormula)
{
  SC_CHECK_PARAM(logicalFormula, ("Invalid logic rule address"));

  ScAddr formulaPremise;
  ScAddr implicationEdge;
  implicationEdge =
      utils::IteratorUtils::getAnyByOutRelation(context, logicalFormula, CoreKeynodes::rrel_main_key_sc_element);
  if (context->HelperCheckEdge(
          inference::InferenceKeynodes::nrel_implication, implicationEdge, ScType::EdgeAccessConstPosPerm))
    formulaPremise = context->GetEdgeSource(implicationEdge);
  return formulaPremise;
}

ScAddr LogicRuleUtils::getFormulaConclusion(ScMemoryContext * context, const ScAddr & logicalFormula)
{
  SC_CHECK_PARAM(logicalFormula, ("Invalid logic rule address"));

  ScAddr formulaConclusion;
  ScAddr implicationEdge;
  implicationEdge =
      utils::IteratorUtils::getAnyByOutRelation(context, logicalFormula, CoreKeynodes::rrel_main_key_sc_element);
  if (context->HelperCheckEdge(
          inference::InferenceKeynodes::nrel_implication, implicationEdge, ScType::EdgeAccessConstPosPerm))
    formulaConclusion = context->GetEdgeTarget(implicationEdge);
  return formulaConclusion;
}

}  // namespace inference
