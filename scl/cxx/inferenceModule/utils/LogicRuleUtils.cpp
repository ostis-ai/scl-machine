/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "LogicRuleUtils.hpp"

#include "keynodes/InferenceKeynodes.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

using namespace scAgentsCommon;

namespace inference
{
ScAddr LogicRuleUtils::getIfStatement(ScMemoryContext * context, const ScAddr & logicRule)
{
  SC_CHECK_PARAM(logicRule, ("Invalid logic rule address"));

  ScAddr ifStatement;
  ScAddr implEdge;
  implEdge = utils::IteratorUtils::getAnyByOutRelation(context, logicRule, CoreKeynodes::rrel_main_key_sc_element);
  if (context->HelperCheckEdge(inference::InferenceKeynodes::nrel_implication, implEdge, ScType::EdgeAccessConstPosPerm))
    ifStatement = context->GetEdgeSource(implEdge);
  return ifStatement;
}

ScAddr LogicRuleUtils::getElseStatement(ScMemoryContext * context, const ScAddr & logicRule)
{
  SC_CHECK_PARAM(logicRule, ("Invalid logic rule address"));

  ScAddr implEdge;
  ScAddr elseStatement;
  implEdge = utils::IteratorUtils::getAnyByOutRelation(context, logicRule, CoreKeynodes::rrel_main_key_sc_element);
  if (context->HelperCheckEdge(inference::InferenceKeynodes::nrel_implication, implEdge, ScType::EdgeAccessConstPosPerm))
    elseStatement = context->GetEdgeTarget(implEdge);
  return elseStatement;
}

}  // namespace inference
