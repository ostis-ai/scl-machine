/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "FormulasIterationStrategyTarget.hpp"

#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "utils/ContainersUtils.hpp"
#include "utils/ReplacementsUtils.hpp"

using namespace inference;

FormulasIterationStrategyTarget::FormulasIterationStrategyTarget(ScMemoryContext * context)
      : FormulasIterationStrategyAbstract(context)
{
}

void FormulasIterationStrategyTarget::setTargetStructure(ScAddr const & otherTargetStructure)
{
  targetStructure = otherTargetStructure;
}

bool FormulasIterationStrategyTarget::applyIterationStrategy(ScAddr const & formulasSet, ScAddr const & outputStructure)
{
  ScAddrVector argumentVector = utils::IteratorUtils::getAllWithType(context, arguments, ScType::Node);
  for (ScAddr const & argument : argumentVector)
  {
    templateSearcher->addParam(argument);
  }
  templateSearcher->setArguments(arguments);

  bool targetAchieved = isTargetAchieved(argumentVector);
  if (targetAchieved)
  {
    SC_LOG_DEBUG("Target is already achieved");
    return false;
  }

  vector<ScAddrQueue> formulasQueuesByPriority = createFormulasQueuesListByPriority(formulasSet);
  if (formulasQueuesByPriority.empty())
  {
    // TODO: а кто ловит то?
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No rule sets found.");
  }

  ScAddrVector checkedFormulas;
  ScAddrQueue uncheckedFormulas;

  ScAddr formula;
  LogicFormulaResult formulaResult;
  SC_LOG_DEBUG("Start rule applying. There is " + to_string(formulasQueuesByPriority.size()) + " formulas sets");
  for (size_t formulasQueueIndex = 0; formulasQueueIndex < formulasQueuesByPriority.size() && !targetAchieved;
       formulasQueueIndex++)
  {
    uncheckedFormulas = formulasQueuesByPriority[formulasQueueIndex];
    SC_LOG_DEBUG(
          "There is " + to_string(uncheckedFormulas.size()) + " formulas in " + to_string(formulasQueueIndex + 1) +
          " set");
    while (!uncheckedFormulas.empty())
    {
      formula = uncheckedFormulas.front();
      SC_LOG_DEBUG("Trying to generate by formula: " + context->HelperGetSystemIdtf(formula));
      formulaResult = useFormula(formula, argumentVector, outputStructure);
      SC_LOG_DEBUG(std::string("Logical formula is ") + (formulaResult.isGenerated ? "generated" : "not generated"));
      if (formulaResult.isGenerated)
      {
        solutionTreeManager->addNode(formula, ReplacementsUtils::getReplacementsToScTemplateParams(
              formulaResult.replacements), ReplacementsUtils::getKeySet(formulaResult.replacements));
        targetAchieved = isTargetAchieved(argumentVector);
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

  return targetAchieved;
}

bool FormulasIterationStrategyTarget::isTargetAchieved(ScAddrVector const & argumentVector)
{
  std::vector<ScTemplateParams> const templateParamsVector =
        templateManager->createTemplateParams(targetStructure, argumentVector);
  return std::any_of(
        templateParamsVector.cbegin(),
        templateParamsVector.cend(),
        [this](ScTemplateParams const & templateParams) {
          return !templateSearcher->searchTemplate(targetStructure, templateParams).empty();
        });
}
