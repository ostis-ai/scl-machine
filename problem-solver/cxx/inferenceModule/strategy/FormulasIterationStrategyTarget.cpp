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

bool FormulasIterationStrategyTarget::applyIterationStrategy(ScAddr const & formulasSet, ScAddr const & outputStructure)
{
  std::vector<ScTemplateParams> const templateParamsVector =
      templateManager->createTemplateParams(targetStructure);
  bool targetAchieved = isTargetAchieved(templateParamsVector);
  if (targetAchieved)
  {
    SC_LOG_DEBUG("Target is already achieved");
    return false;
  }

  vector<ScAddrQueue> formulasQueuesByPriority = createFormulasQueuesListByPriority(formulasSet);
  if (formulasQueuesByPriority.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No rule sets found.");
  }

  // Extend input structures vector with outputStructure to find target with generated elements
  ScAddrVector inputStructures = templateSearcher->getInputStructures();
  inputStructures.push_back(outputStructure);
  templateSearcher->setInputStructures(inputStructures);

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
      SC_LOG_DEBUG("Trying to generate by formula: " << context->HelperGetSystemIdtf(formula));
      formulaResult = useFormula(formula, outputStructure);
      SC_LOG_DEBUG("Logical formula is " << (formulaResult.isGenerated ? "generated" : "not generated"));
      if (formulaResult.isGenerated)
      {
        if (generateSolutionTree)
        {
          std::set<std::string> varNames;
          ReplacementsUtils::getKeySet(formulaResult.replacements, varNames);
          solutionTreeManager->addNode(formula, ReplacementsUtils::getReplacementsToScTemplateParams(
                                                    formulaResult.replacements), varNames);
        }
        // We need to check target with result generated replacements, not with input
        targetAchieved = isTargetAchieved(ReplacementsUtils::getReplacementsToScTemplateParams(formulaResult.replacements));
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

void FormulasIterationStrategyTarget::setTargetStructure(ScAddr const & otherTargetStructure)
{
  targetStructure = otherTargetStructure;
}

bool FormulasIterationStrategyTarget::isTargetAchieved(std::vector<ScTemplateParams> const & templateParamsVector)
{
  std::set<std::string> varNames;
  templateSearcher->getVarNames(targetStructure, varNames);
  return std::any_of(
        templateParamsVector.cbegin(),
        templateParamsVector.cend(),
        [this, &varNames](ScTemplateParams const & templateParams) -> bool {
          Replacements result;
          templateSearcher->searchTemplate(targetStructure, templateParams, varNames, result);
          return !result.empty();
        });
}
