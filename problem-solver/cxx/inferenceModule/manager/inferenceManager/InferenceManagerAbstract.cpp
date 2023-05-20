/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceManagerAbstract.hpp"

#include <utility>

#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "manager/templateManager/TemplateManagerFixedArguments.hpp"
#include "utils/ContainersUtils.hpp"
#include "logic/LogicExpression.hpp"

using namespace inference;

InferenceManagerAbstract::InferenceManagerAbstract(ScMemoryContext * context)
  : context(context)
{
}

void InferenceManagerAbstract::setTemplateSearcher(std::shared_ptr<TemplateSearcherAbstract> searcher)
{
  templateSearcher = std::move(searcher);
}

void InferenceManagerAbstract::setTemplateManager(std::shared_ptr<TemplateManagerAbstract> manager)
{
  templateManager = std::move(manager);
}

void InferenceManagerAbstract::setSolutionTreeManager(std::shared_ptr<SolutionTreeManagerAbstract> manager)
{
  solutionTreeManager = std::move(manager);
}

std::shared_ptr<SolutionTreeManagerAbstract> InferenceManagerAbstract::getSolutionTreeManager()
{
  return solutionTreeManager;
}

vector<ScAddrQueue> InferenceManagerAbstract::createFormulasQueuesListByPriority(ScAddr const & formulasSet)
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

ScAddrQueue InferenceManagerAbstract::createQueue(ScAddr const & set)
{
  ScAddrQueue queue;
  ScAddrVector elementList = utils::IteratorUtils::getAllWithType(context, set, ScType::Node);

  ContainersUtils::addToQueue(elementList, queue);
  return queue;
}

/**
 * @brief Build logic expression tree and compute it
 * @param formula is a logical formula to use (more often non-atomic formula is an implication, generating conclusion)
 * @param outputStructure is a structure to generate new knowledge in
 * @returns LogicFormulaResult {bool: value, bool: isGenerated, Replacements: replacements}
 */
LogicFormulaResult InferenceManagerAbstract::useFormula(ScAddr const & formula, ScAddr const & outputStructure)
{
  ScAddr const & formulaRoot = utils::IteratorUtils::getAnyByOutRelation(
      context, formula, scAgentsCommon::CoreKeynodes::rrel_main_key_sc_element);
  if (!formulaRoot.IsValid())
  {
    return {false, false, {}};
  }

  // Choose template manager according to the formula specification (if fixed arguments exist)
  ScAddr const & firstFixedArgument =
      utils::IteratorUtils::getAnyByOutRelation(context, formula, scAgentsCommon::CoreKeynodes::rrel_1);
  if (firstFixedArgument.IsValid())
  {
    formTemplateManagerFixedArguments(formula, firstFixedArgument);
  }
  else
  {
    resetTemplateManager(std::make_shared<TemplateManager>(context));
  }

  LogicExpression logicExpression(context, templateSearcher, templateManager, solutionTreeManager, outputStructure);

  std::shared_ptr<LogicExpressionNode> expressionRoot = logicExpression.build(formulaRoot);
  expressionRoot->setArgumentVector(templateManager->getArguments());
  expressionRoot->setOutputStructureElements(outputStructureElements);

  LogicFormulaResult formulaResult;
  expressionRoot->compute(formulaResult);

  return formulaResult;
}

/// Form formula fixed arguments from rrel_1, rrel_2 etc. to create template params. Used only in
/// 'TemplateManagerFixedArguments'
void InferenceManagerAbstract::fillFormulaFixedArgumentsIdentifiers(
    ScAddr const & formula,
    ScAddr const & firstFixedArgument) const
{
  std::string const firstFixedArgumentIdentifier = context->HelperGetSystemIdtf(firstFixedArgument);
  if (!firstFixedArgumentIdentifier.empty())
  {
    templateManager->addFixedArgumentIdentifier(firstFixedArgumentIdentifier);
  }

  // TODO(MksmOrlov): make nrel_basic_sequence oriented set processing
  size_t const maxFixedArgumentsCount = 10;
  ScAddr currentFixedArgument;
  ScAddr currentRoleRelation;
  std::string currentFixedArgumentIdentifier;
  for (size_t i = 2; i <= maxFixedArgumentsCount; i++)
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

void InferenceManagerAbstract::formTemplateManagerFixedArguments(
    ScAddr const & formula,
    ScAddr const & firstFixedArgument)
{
  resetTemplateManager(std::make_shared<TemplateManagerFixedArguments>(context));
  fillFormulaFixedArgumentsIdentifiers(formula, firstFixedArgument);
}

void InferenceManagerAbstract::resetTemplateManager(std::shared_ptr<TemplateManagerAbstract> otherTemplateManager)
{
  otherTemplateManager->setArguments(templateManager->getArguments());
  otherTemplateManager->setGenerationType(templateManager->getGenerationType());
  otherTemplateManager->setReplacementsUsingType(templateManager->getReplacementsUsingType());
  templateManager = std::move(otherTemplateManager);
}
