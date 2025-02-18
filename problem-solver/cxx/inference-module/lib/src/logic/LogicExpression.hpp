/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "classifier/FormulaClassifier.hpp"

#include <sc-memory/sc_template.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>

#include "inference/inference_keynodes.hpp"
#include "inference/template_manager.hpp"
#include "inference/replacements_utils.hpp"

#include "manager/solution-tree-manager/SolutionTreeManager.hpp"

#include "searcher/template-searcher/TemplateSearcherAbstract.hpp"

#include "LogicExpressionNode.hpp"

using namespace inference;

class LogicExpression
{
public:
  LogicExpression(
      ScMemoryContext * context,
      std::shared_ptr<TemplateSearcherAbstract> templateSearcher,
      std::shared_ptr<TemplateManagerAbstract> templateManager,
      std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager,
      ScAddr const & outputStructure);

  std::shared_ptr<LogicExpressionNode> build(ScAddr const & formula);

  std::shared_ptr<LogicExpressionNode> buildAtomicFormula(ScAddr const & formula);
  std::shared_ptr<LogicExpressionNode> buildConjunctionFormula(ScAddr const & formula);
  std::shared_ptr<LogicExpressionNode> buildDisjunctionFormula(ScAddr const & formula);
  std::shared_ptr<LogicExpressionNode> buildNegationFormula(ScAddr const & formula);
  std::shared_ptr<LogicExpressionNode> buildImplicationArcFormula(ScAddr const & formula);
  std::shared_ptr<LogicExpressionNode> buildImplicationTupleFormula(ScAddr const & formula);
  std::shared_ptr<LogicExpressionNode> buildEquivalenceEdgeFormula(ScAddr const & formula);
  std::shared_ptr<LogicExpressionNode> buildEquivalenceTupleFormula(ScAddr const & formula);

  OperatorLogicExpressionNode::OperandsVector resolveTupleOperands(ScAddr const & tuple);
  OperatorLogicExpressionNode::OperandsVector resolveConnectorOperands(ScAddr const & connector);
  OperatorLogicExpressionNode::OperandsVector resolveOperandsForImplicationTuple(ScAddr const & tuple);

private:
  ScMemoryContext * context;
  std::vector<ScTemplateParams> paramsSet;

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  std::shared_ptr<TemplateManagerAbstract> templateManager;
  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;

  ScAddr outputStructure;
};
