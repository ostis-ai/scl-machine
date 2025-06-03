/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "inference/inference_manager_abstract.hpp"

#include <utility>

#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "inference/containers_utils.hpp"

#include "manager/template-manager/TemplateManagerFixedArguments.hpp"

#include "logic/LogicExpression.hpp"

using namespace inference;

InferenceManagerAbstract::InferenceManagerAbstract(ScMemoryContext * context, utils::ScLogger * logger)
  : context(context), logger(logger)
{
}

void InferenceManagerAbstract::SetTemplateSearcher(std::shared_ptr<TemplateSearcherAbstract> searcher)
{
  templateSearcher = std::move(searcher);
}

void InferenceManagerAbstract::SetTemplateManager(std::shared_ptr<TemplateManagerAbstract> manager)
{
  templateManager = std::move(manager);
}

void InferenceManagerAbstract::SetSolutionTreeManager(std::shared_ptr<SolutionTreeManagerAbstract> manager)
{
  solutionTreeManager = std::move(manager);
}

std::shared_ptr<SolutionTreeManagerAbstract> InferenceManagerAbstract::GetSolutionTreeManager()
{
  return solutionTreeManager;
}

std::vector<ScAddrQueue> InferenceManagerAbstract::CreateFormulasQueuesListByPriority(ScAddr const & formulasSet)
{
  std::vector<ScAddrQueue> formulasQueuesList;

  ScAddr setOfFormulas = utils::IteratorUtils::getAnyByOutRelation(context, formulasSet, ScKeynodes::rrel_1);
  while (setOfFormulas.IsValid())
  {
    formulasQueuesList.push_back(CreateQueue(setOfFormulas));
    setOfFormulas = utils::IteratorUtils::getNextFromSet(context, formulasSet, setOfFormulas);
  }

  return formulasQueuesList;
}

ScAddrQueue InferenceManagerAbstract::CreateQueue(ScAddr const & set)
{
  ScAddrQueue queue;
  ScAddrVector elementList = utils::IteratorUtils::getAllWithType(context, set, ScType::Node);

  ContainersUtils::AddToQueue(elementList, queue);
  return queue;
}

/**
 * @brief Build logic expression tree and compute it
 * @param formula is a logical formula to use (more often non-atomic formula is an implication, generating conclusion)
 * @param outputStructure is a structure to generate new knowledge in
 * @returns LogicFormulaResult {bool: value, bool: isGenerated, Replacements: replacements}
 */
LogicFormulaResult InferenceManagerAbstract::UseFormula(ScAddr const & formula, ScAddr const & outputStructure)
{
  ScAddr const & formulaRoot =
      utils::IteratorUtils::getAnyByOutRelation(context, formula, ScKeynodes::rrel_main_key_sc_element);
  if (!formulaRoot.IsValid())
  {
    return {false, false, {}};
  }

  // Choose template manager according to the formula specification (if fixed arguments exist)
  ScAddr const & firstFixedArgument = utils::IteratorUtils::getAnyByOutRelation(context, formula, ScKeynodes::rrel_1);
  if (firstFixedArgument.IsValid())
  {
    FormTemplateManagerFixedArguments(formula, firstFixedArgument);
  }
  else
  {
    ResetTemplateManager(std::make_shared<TemplateManager>(context));
  }

  LogicExpression logicExpression(context, logger, templateSearcher, templateManager, solutionTreeManager, outputStructure);

  std::shared_ptr<LogicExpressionNode> expressionRoot = logicExpression.build(formulaRoot);
  expressionRoot->setArgumentVector(templateManager->GetArguments());
  expressionRoot->setOutputStructureElements(outputStructureElements);

  LogicFormulaResult formulaResult;
  expressionRoot->compute(formulaResult);

  return formulaResult;
}

/// Form formula fixed arguments from rrel_1, rrel_2 etc. to create template params. Used only in
/// 'TemplateManagerFixedArguments'
void InferenceManagerAbstract::FillFormulaFixedArgumentsIdentifiers(
    ScAddr const & formula,
    ScAddr const & firstFixedArgument) const
{
  templateManager->AddFixedArgument(firstFixedArgument);

  // TODO(MksmOrlov): make nrel_basic_sequence oriented set processing
  size_t const maxFixedArgumentsCount = 10;
  ScAddr currentFixedArgument;
  ScAddr currentRoleRelation;
  for (size_t i = 2; i <= maxFixedArgumentsCount; i++)
  {
    currentRoleRelation = utils::IteratorUtils::getRoleRelation(context, i);
    currentFixedArgument = utils::IteratorUtils::getAnyByOutRelation(context, formula, currentRoleRelation);
    if (!currentFixedArgument.IsValid())
    {
      break;
    }
    templateManager->AddFixedArgument(currentFixedArgument);
  }
}

void InferenceManagerAbstract::FormTemplateManagerFixedArguments(
    ScAddr const & formula,
    ScAddr const & firstFixedArgument)
{
  ResetTemplateManager(std::make_shared<TemplateManagerFixedArguments>(context));
  FillFormulaFixedArgumentsIdentifiers(formula, firstFixedArgument);
}

void InferenceManagerAbstract::ResetTemplateManager(std::shared_ptr<TemplateManagerAbstract> otherTemplateManager)
{
  otherTemplateManager->SetArguments(templateManager->GetArguments());
  otherTemplateManager->SetGenerationType(templateManager->GetGenerationType());
  otherTemplateManager->SetReplacementsUsingType(templateManager->GetReplacementsUsingType());
  otherTemplateManager->SetFillingType(templateManager->GetFillingType());
  templateManager = std::move(otherTemplateManager);
}
InferenceManagerAbstract::~InferenceManagerAbstract()
{
  outputStructureElements.clear();
}
