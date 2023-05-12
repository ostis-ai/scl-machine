/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DirectInferenceManager.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>

#include <algorithm>

#include "utils/ContainersUtils.hpp"
#include "logic/LogicExpression.hpp"

using namespace inference;

DirectInferenceManager::DirectInferenceManager(ScMemoryContext * ms_context)
  : ms_context(ms_context)
{
  solutionTreeManager = std::make_shared<SolutionTreeManager>(ms_context);
  templateManager = std::make_shared<TemplateManager>(ms_context);
  templateSearcher = std::make_shared<TemplateSearcher>(ms_context);
}

ScAddr DirectInferenceManager::applyInference(
    ScAddr const & targetStructure,
    ScAddr const & formulasSet,
    ScAddr const & arguments,
    ScAddr const & inputStructure)
{
  ScAddr outputStructure = ms_context->CreateNode(ScType::NodeConstStruct);
  ScAddrVector argumentVector = utils::IteratorUtils::getAllWithType(ms_context, arguments, ScType::Node);
  templateManager->setArguments(argumentVector);

  bool targetAchieved = isTargetAchieved(targetStructure, argumentVector);
  if (targetAchieved)
  {
    SC_LOG_DEBUG("Target is already achieved");
    return solutionTreeManager->createSolution(outputStructure, targetAchieved);
  }

  vector<ScAddrQueue> formulasQueuesByPriority = createFormulasQueuesListByPriority(formulasSet);
  if (formulasQueuesByPriority.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No rule sets found.");
  }

  ScAddrVector checkedFormulas;
  ScAddrQueue uncheckedFormulas;

  ScAddr formula;
  LogicFormulaResult formulaResult;
  SC_LOG_DEBUG("Start rule applying. There is " << formulasQueuesByPriority.size() << " formulas sets");
  for (size_t formulasQueueIndex = 0; formulasQueueIndex < formulasQueuesByPriority.size() && !targetAchieved;
       formulasQueueIndex++)
  {
    uncheckedFormulas = formulasQueuesByPriority[formulasQueueIndex];
    SC_LOG_DEBUG(
        "There is " << uncheckedFormulas.size() << " formulas in " << (formulasQueueIndex + 1) << " set");
    while (!uncheckedFormulas.empty())
    {
      formula = uncheckedFormulas.front();
      SC_LOG_DEBUG("Trying to generate by formula: " << ms_context->HelperGetSystemIdtf(formula));
      formulaResult = useFormula(formula, argumentVector, outputStructure);
      SC_LOG_DEBUG("Logical formula is " << (formulaResult.isGenerated ? "generated" : "not generated"));
      if (formulaResult.isGenerated)
      {
        std::set<std::string> varNames;
        ReplacementsUtils::getKeySet(formulaResult.replacements, varNames);
        solutionTreeManager->addNode(formula, ReplacementsUtils::getReplacementsToScTemplateParams(
            formulaResult.replacements), varNames);
        targetAchieved = isTargetAchieved(targetStructure, argumentVector);
        if (targetAchieved)
        {
          SC_LOG_DEBUG("Target achieved");
          break;
        }
        else
        {
          ContainersUtils::addToQueue(checkedFormulas, uncheckedFormulas);
          formulasQueueIndex = 0;
          checkedFormulas.clear();
        }
      }
      else
      {
        checkedFormulas.push_back(formula);
      }

      uncheckedFormulas.pop();
    }
  }

  return solutionTreeManager->createSolution(outputStructure, targetAchieved);
}

ScAddrQueue DirectInferenceManager::createQueue(ScAddr const & set)
{
  ScAddrQueue queue;
  ScAddrVector elementList = utils::IteratorUtils::getAllWithType(ms_context, set, ScType::Node);

  ContainersUtils::addToQueue(elementList, queue);
  return queue;
}

LogicFormulaResult DirectInferenceManager::useFormula(
    ScAddr const & rule,
    ScAddrVector & argumentVector,
    ScAddr const & outputStructure)
{
  LogicFormulaResult formulaResult = {false, false, {}};
  ScAddr const formulaRoot =
      utils::IteratorUtils::getAnyByOutRelation(ms_context, rule, scAgentsCommon::CoreKeynodes::rrel_main_key_sc_element);
  if (!formulaRoot.IsValid())
    return {false, false, {}};

  LogicExpression logicExpression(
        ms_context,
        templateSearcher,
        templateManager,
        solutionTreeManager,
        outputStructure);

  std::shared_ptr<LogicExpressionNode> expressionRoot = logicExpression.build(formulaRoot);
  expressionRoot->setArgumentVector(argumentVector);

  LogicFormulaResult result = expressionRoot->compute(formulaResult);

  if (result.isGenerated && (ReplacementsUtils::getColumnsAmount(result.replacements)) != 1)
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "replacements have " << ReplacementsUtils::getColumnsAmount(result.replacements) << " replacements");

  return result;
}

vector<ScAddrQueue> DirectInferenceManager::createFormulasQueuesListByPriority(ScAddr const & formulasSet)
{
  vector<ScAddrQueue> formulasQueuesList;

  ScAddr setOfFormulas =
      utils::IteratorUtils::getAnyByOutRelation(ms_context, formulasSet, scAgentsCommon::CoreKeynodes::rrel_1);
  while (setOfFormulas.IsValid())
  {
    formulasQueuesList.push_back(createQueue(setOfFormulas));
    setOfFormulas = utils::IteratorUtils::getNextFromSet(ms_context, formulasSet, setOfFormulas);
  }

  return formulasQueuesList;
}

bool DirectInferenceManager::isTargetAchieved(ScAddr const & targetStructure, ScAddrVector const & argumentVector)
{
  std::vector<ScTemplateParams> const templateParamsVector =
      templateManager->createTemplateParams(targetStructure);
  std::set<std::string> varNames;
  templateSearcher->getVarNames(targetStructure, varNames);
  return std::any_of(
      templateParamsVector.cbegin(),
      templateParamsVector.cend(),
      [this, &targetStructure, &varNames](ScTemplateParams const & templateParams) {
        Replacements result;
        templateSearcher->searchTemplate(targetStructure, templateParams, varNames, result);
        return !result.empty();
      });
}
