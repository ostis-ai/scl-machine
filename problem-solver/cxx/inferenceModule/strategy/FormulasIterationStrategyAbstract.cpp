/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "FormulasIterationStrategyAbstract.hpp"

#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "utils/ContainersUtils.hpp"
#include "logic/LogicExpression.hpp"

using namespace inference;

FormulasIterationStrategyAbstract::FormulasIterationStrategyAbstract(ScMemoryContext * context)
      : context(context)
{
  // Now we don't have several implementations for SolutionTreeManager and TemplateManager
  solutionTreeManager = std::make_unique<SolutionTreeManager>(context);
  templateManager = std::make_unique<TemplateManager>(context);
}

void FormulasIterationStrategyAbstract::setTemplateSearcher(std::unique_ptr<TemplateSearcherAbstract> searcher)
{
  templateSearcher = std::move(searcher);
}

void FormulasIterationStrategyAbstract::setTemplateManager(std::unique_ptr<TemplateManager> manager)
{
  templateManager = std::move(manager);
}

void FormulasIterationStrategyAbstract::setSolutionTreeManager(std::unique_ptr<SolutionTreeManager> manager)
{
  solutionTreeManager = std::move(manager);
}

void FormulasIterationStrategyAbstract::setArguments(ScAddr const & otherArguments)
{
  arguments = otherArguments;
}

std::unique_ptr<SolutionTreeManager> FormulasIterationStrategyAbstract::getSolutionTreeManager()
{
  return std::move(solutionTreeManager);
}

vector<ScAddrQueue> FormulasIterationStrategyAbstract::createFormulasQueuesListByPriority(ScAddr const & formulasSet)
{
  vector<ScAddrQueue> formulasQueuesList;

  ScAddr setOfFormulas =
        utils::IteratorUtils::getAnyByOutRelation(context, formulasSet, scAgentsCommon::CoreKeynodes::rrel_1);
  while (setOfFormulas.IsValid())
  {
    formulasQueuesList.push_back(createQueue(setOfFormulas));
    setOfFormulas = utils::IteratorUtils::getNextFromSet(context, formulasSet, setOfFormulas);
  }

  return formulasQueuesList;
}

ScAddrQueue FormulasIterationStrategyAbstract::createQueue(ScAddr const & set)
{
  ScAddrQueue queue;
  ScAddrVector elementList = utils::IteratorUtils::getAllWithType(context, set, ScType::Node);

  ContainersUtils::addToQueue(elementList, queue);
  return queue;
}

LogicFormulaResult FormulasIterationStrategyAbstract::useFormula(
      ScAddr const & rule,
      ScAddrVector & argumentVector,
      ScAddr const & outputStructure)
{
  LogicFormulaResult formulaResult = {false, false, {}};
  ScAddr const formulaRoot =
        utils::IteratorUtils::getAnyByOutRelation(context, rule, InferenceKeynodes::rrel_main_key_sc_element);
  if (!formulaRoot.IsValid())
    return {false, false, {}};

  LogicExpression logicExpression(
        context,
        std::move(templateSearcher),
        std::move(templateManager),
        std::move(solutionTreeManager),
        outputStructure,
        rule);

  std::shared_ptr<LogicExpressionNode> expressionRoot = logicExpression.build(formulaRoot);
  expressionRoot->setArgumentVector(argumentVector);

  LogicFormulaResult result = expressionRoot->compute(formulaResult);

  if (result.isGenerated && (ReplacementsUtils::getColumnsAmount(result.replacements)) != 1)
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "replacements have " << ReplacementsUtils::getColumnsAmount(result.replacements) << " replacements");

  return result;
}
