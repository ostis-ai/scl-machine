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
  this->solutionTreeGenerator = std::make_unique<SolutionTreeGenerator>(ms_context);
  this->templateManager = std::make_unique<TemplateManager>(ms_context);
  this->templateSearcher = std::make_unique<TemplateSearcher>(ms_context);
}

ScAddr DirectInferenceManager::applyInference(
    ScAddr const & targetStructure,
    ScAddr const & formulasSet,
    ScAddr const & inputStructure,
    ScAddr const & outputStructure)
{
  ScAddrVector argumentVector = utils::IteratorUtils::getAllWithType(ms_context, inputStructure, ScType::Node);
  for (ScAddr const & argument : argumentVector)
  {
    templateSearcher->addParam(argument);
  }
  templateSearcher->setInputStructure(inputStructure);

  bool targetAchieved = isTargetAchieved(targetStructure, argumentVector);
  if (targetAchieved)
  {
    SC_LOG_DEBUG("Target is already achieved");
    return this->solutionTreeGenerator->createSolution(targetAchieved);
  }

  vector<ScAddrQueue> formulasQueuesByPriority = createFormulasQueuesListByPriority(formulasSet);
  if (formulasQueuesByPriority.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No rule sets found.");
  }

  ScAddrVector checkedFormulas;
  ScAddrQueue uncheckedFormulas;

  ScAddr formula;
  bool isGenerated;
  SC_LOG_DEBUG("Start rule applying. There is " + to_string(formulasQueuesByPriority.size()) + " formulas sets");
  for (size_t formulasQueueIndex = 0; formulasQueueIndex < formulasQueuesByPriority.size() && !targetAchieved;
       formulasQueueIndex++)
  {
    uncheckedFormulas = formulasQueuesByPriority[formulasQueueIndex];
    if (uncheckedFormulas.empty())
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No rules found in set by priority.");
    }
    SC_LOG_DEBUG(
        "There is " + to_string(uncheckedFormulas.size()) + " formulas in " + to_string(formulasQueueIndex + 1) +
        " set");
    while (!uncheckedFormulas.empty())
    {
      formula = uncheckedFormulas.front();
      clearSatisfiabilityInformation(formula, inputStructure);
      SC_LOG_DEBUG("Trying to generate by formula: " + ms_context->HelperGetSystemIdtf(formula));
      isGenerated = useFormula(formula, argumentVector, outputStructure);
      SC_LOG_DEBUG(std::string("Logical formulas is ") + (isGenerated ? "generated" : "not generated"));
      if (isGenerated)
      {
        addSatisfiabilityInformation(formula, inputStructure, true);
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
        addSatisfiabilityInformation(formula, inputStructure, false);
        checkedFormulas.push_back(formula);
      }

      uncheckedFormulas.pop();
    }
  }

  return this->solutionTreeGenerator->createSolution(targetAchieved);
}

ScAddrQueue DirectInferenceManager::createQueue(ScAddr const & set)
{
  ScAddrQueue queue;
  ScAddrVector elementList = utils::IteratorUtils::getAllWithType(ms_context, set, ScType::Node);

  ContainersUtils::addToQueue(elementList, queue);
  return queue;
}

bool DirectInferenceManager::useFormula(
    ScAddr const & rule,
    ScAddrVector /*const*/ & argumentVector,
    ScAddr const & outputStructure)
{
  LogicFormulaResult formulaResult = {false, false, {}};
  ScAddr const formulaRoot =
      utils::IteratorUtils::getAnyByOutRelation(ms_context, rule, InferenceKeynodes::rrel_main_key_sc_element);
  if (!formulaRoot.IsValid())
    return false;

  LogicExpression logicExpression(
      ms_context, templateSearcher.get(), templateManager.get(), argumentVector, outputStructure);

  unique_ptr<LogicExpressionNode> expressionRoot = logicExpression.build(formulaRoot);
  LogicFormulaResult result = expressionRoot->compute(formulaResult);

  return result.isGenerated;
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

bool DirectInferenceManager::generateStatement(ScAddr const & statement, ScTemplateParams const & templateParams)
{
  bool result = false;
  ScTemplate searchTemplate;
  ms_context->HelperBuildTemplate(searchTemplate, statement, templateParams);
  ScTemplateSearchResult templateSearchResult;
  if (!ms_context->HelperSearchTemplate(searchTemplate, templateSearchResult))
  {
    ScTemplate statementTemplate;
    ms_context->HelperBuildTemplate(statementTemplate, statement);
    ScTemplateGenResult templateGenResult;
    result = ms_context->HelperGenTemplate(statementTemplate, templateGenResult, templateParams);
  }
  return result;
}

bool DirectInferenceManager::isTargetAchieved(ScAddr const & targetStructure, ScAddrVector const & argumentVector)
{
  std::vector<ScTemplateParams> const templateParamsVector =
      templateManager->createTemplateParams(targetStructure, argumentVector);
  return std::any_of(
      templateParamsVector.cbegin(),
      templateParamsVector.cend(),
      [this, &targetStructure](ScTemplateParams const & templateParams) {
        return !templateSearcher->searchTemplate(targetStructure, templateParams).empty();
      });
}

void DirectInferenceManager::clearSatisfiabilityInformation(ScAddr const & formula, ScAddr const & inputStructure)
{
  ScIterator5Ptr satisfiabilityIterator = ms_context->Iterator5(
      formula, ScType::EdgeDCommon, inputStructure, ScType::EdgeAccess, InferenceKeynodes::nrel_satisfiable_formula);

  while (satisfiabilityIterator->Next())
    ms_context->EraseElement(satisfiabilityIterator->Get(1));
}

void DirectInferenceManager::addSatisfiabilityInformation(
    ScAddr const & formula,
    ScAddr const & inputStructure,
    bool isSatisfiable)
{
  clearSatisfiabilityInformation(formula, inputStructure);
  ScAddr const satisfiableRelationEdge = ms_context->CreateEdge(ScType::EdgeDCommonConst, formula, inputStructure);
  ScType const accessArcType = (isSatisfiable ? ScType::EdgeAccessConstPosTemp : ScType::EdgeAccessConstNegTemp);
  ms_context->CreateEdge(accessArcType, InferenceKeynodes::nrel_satisfiable_formula, satisfiableRelationEdge);
}
