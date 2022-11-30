/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_template.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"

#include "LogicExpressionNode.hpp"

#include "utils/ReplacementsUtils.hpp"
#include "manager/TemplateManager.hpp"
#include "searcher/TemplateSearcher.hpp"
#include "classifier/FormulaClassifier.hpp"

using namespace inference;

class LogicExpression
{
public:
  LogicExpression(
      ScMemoryContext * context,
      TemplateSearcher * templateSearcher,
      TemplateManager * templateManager,
      ScAddrVector argumentVector);
  LogicExpression(
      ScMemoryContext * context,
      TemplateSearcher * templateSearcher,
      TemplateManager * templateManager,
      ScAddrVector argumentVector,
      ScAddr outputStructure);

  std::unique_ptr<LogicExpressionNode> build(ScAddr const & node);

  OperatorLogicExpressionNode::OperandsVector resolveTupleOperands(ScAddr const & tuple);

  OperatorLogicExpressionNode::OperandsVector resolveEdgeOperands(ScAddr const & edge);

  OperatorLogicExpressionNode::OperandsVector resolveOperandsForImplicationTuple(ScAddr const & tuple);

private:
  ScMemoryContext * context;
  std::vector<ScTemplateParams> paramsSet;

  TemplateSearcher * templateSearcher;
  TemplateManager * templateManager;
  ScAddrVector argumentVector;

  ScAddr inputStructure;
  ScAddr outputStructure;
};
