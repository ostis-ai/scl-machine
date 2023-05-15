/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "FormulasIterationStrategyAbstract.hpp"

#include <utility>

#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "utils/ContainersUtils.hpp"
#include "logic/LogicExpression.hpp"

using namespace inference;

FormulasIterationStrategyAbstract::FormulasIterationStrategyAbstract(ScMemoryContext * context)
      : context(context)
{
}

void FormulasIterationStrategyAbstract::setTemplateSearcher(std::shared_ptr<TemplateSearcherAbstract> searcher)
{
  templateSearcher = std::move(searcher);
}

void FormulasIterationStrategyAbstract::setTemplateManager(std::shared_ptr<TemplateManagerAbstract> manager)
{
  templateManager = std::move(manager);
}

void FormulasIterationStrategyAbstract::setSolutionTreeManager(std::shared_ptr<SolutionTreeManagerAbstract> manager)
{
  solutionTreeManager = std::move(manager);
}

std::shared_ptr<SolutionTreeManagerAbstract> FormulasIterationStrategyAbstract::getSolutionTreeManager()
{
  return solutionTreeManager;
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

/**
 * @brief Build logic expression tree and compute it
 * @param formula is a logical formula to use (more often non-atomic formula is an implication, generating conclusion)
 * @param argumentVector is a vector of ScAddrs to use in atomic sub formulas of `formula`. May be empty to use all existing sc-elements
 * @param outputStructure is a structure to generate new knowledge in
 * @returns LogicFormulaResult {bool: value, bool: isGenerated, Replacements: replacements}
 */
LogicFormulaResult FormulasIterationStrategyAbstract::useFormula(
      ScAddr const & formula,
      ScAddr const & outputStructure)
{
  ScAddr const & formulaRoot =
        utils::IteratorUtils::getAnyByOutRelation(context, formula, scAgentsCommon::CoreKeynodes::rrel_main_key_sc_element);
  if (!formulaRoot.IsValid())
  {
    return {false, false, {}};
  }

  fillFormulaFixedArgumentsIdentifiers(formula);
  LogicExpression logicExpression(
        context,
        templateSearcher,
        templateManager,
        solutionTreeManager,
        outputStructure);

  std::shared_ptr<LogicExpressionNode> expressionRoot = logicExpression.build(formulaRoot);
  expressionRoot->setArgumentVector(templateManager->getArguments());
  expressionRoot->setOutputStructureElements(outputStructureElements);

  LogicFormulaResult formulaResult;
  expressionRoot->compute(formulaResult);

  return formulaResult;
}

void FormulasIterationStrategyAbstract::fillFormulaFixedArgumentsIdentifiers(ScAddr const & formula) const
{
  // TODO(MksmOrlov): make nrel_basic_sequence oriented set processing
  size_t const maxFixedArgumentsCount = 10;
  ScAddr currentFixedArgument;
  ScAddr currentRoleRelation;
  std::string currentFixedArgumentIdentifier;
  for (size_t i = 1; i <= maxFixedArgumentsCount; i++)
  {
    currentRoleRelation = utils::IteratorUtils::getRoleRelation(context, i);
    currentFixedArgument = utils::IteratorUtils::getAnyByOutRelation(context, formula, currentRoleRelation);
    if (!currentFixedArgument.IsValid())
    {
      break;
    }
    currentFixedArgumentIdentifier = context->HelperGetSystemIdtf(currentFixedArgument);
    if (!currentFixedArgumentIdentifier.empty())
    {
      templateManager->addFixedArgumentIdentifier(currentFixedArgumentIdentifier);
    }
  }
}
