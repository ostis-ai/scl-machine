/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DirectInferenceManagerTarget.hpp"

#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "utils/ContainersUtils.hpp"
#include "utils/ReplacementsUtils.hpp"

using namespace inference;

DirectInferenceManagerTarget::DirectInferenceManagerTarget(ScMemoryContext * context)
  : InferenceManagerAbstract(context)
{
}

bool DirectInferenceManagerTarget::applyInference(InferenceParams const & inferenceParamsConfig)
{
  templateManager->setArguments(inferenceParamsConfig.arguments);
  templateSearcher->setInputStructures(inferenceParamsConfig.inputStructures);
  setTargetStructure(inferenceParamsConfig.targetStructure);

  std::vector<ScTemplateParams> const templateParamsVector = templateManager->createTemplateParams(targetStructure);
  bool targetAchieved = isTargetAchieved(templateParamsVector);
  if (targetAchieved)
  {
    SC_LOG_DEBUG("Target is already achieved");
    return false;
  }

  std::vector<ScAddrQueue> formulasQueuesByPriority = createFormulasQueuesListByPriority(inferenceParamsConfig.formulasSet);
  if (formulasQueuesByPriority.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No formulas sets found.");
  }

  // Extend input structures vector with outputStructure to search target with generated elements
  ScAddrUnorderedSet inputStructures = templateSearcher->getInputStructures();
  inputStructures.insert(inferenceParamsConfig.outputStructure);
  templateSearcher->setInputStructures(inputStructures);

  ScAddrVector checkedFormulas;
  ScAddrQueue uncheckedFormulas;

  ScAddr formula;
  LogicFormulaResult formulaResult;
  SC_LOG_DEBUG("Start formulas applying. There is " << formulasQueuesByPriority.size() << " formulas sets");
  for (size_t formulasQueueIndex = 0; formulasQueueIndex < formulasQueuesByPriority.size() && !targetAchieved;
       formulasQueueIndex++)
  {
    uncheckedFormulas = formulasQueuesByPriority[formulasQueueIndex];
    SC_LOG_DEBUG("There is " << uncheckedFormulas.size() << " formulas in " << (formulasQueueIndex + 1) << " set");
    while (!uncheckedFormulas.empty())
    {
      formula = uncheckedFormulas.front();
      SC_LOG_DEBUG("Trying to generate by formula: " << context->GetElementSystemIdentifier(formula));
      formulaResult = useFormula(formula, inferenceParamsConfig.outputStructure);
      SC_LOG_DEBUG("Logical formula is " << (formulaResult.isGenerated ? "generated" : "not generated"));
      if (formulaResult.isGenerated)
      {
        solutionTreeManager->addNode(formula, formulaResult.replacements);
        // We need to check target with result generated replacements, not with input
        std::vector<ScTemplateParams> paramsVector;
        ReplacementsUtils::getReplacementsToScTemplateParams(formulaResult.replacements, paramsVector);
        targetAchieved = isTargetAchieved(paramsVector);
        if (targetAchieved)
        {
          SC_LOG_DEBUG("Target is achieved");
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

void DirectInferenceManagerTarget::setTargetStructure(ScAddr const & otherTargetStructure)
{
  targetStructure = otherTargetStructure;
}

bool DirectInferenceManagerTarget::isTargetAchieved(std::vector<ScTemplateParams> const & templateParamsVector)
{
  ScAddrUnorderedSet variables;
  templateSearcher->getVariables(targetStructure, variables);
  return std::any_of(
      templateParamsVector.cbegin(),
      templateParamsVector.cend(),
      [this, &variables](ScTemplateParams const & templateParams) -> bool {
        Replacements result;
        templateSearcher->searchTemplate(targetStructure, templateParams, variables, result);
        return !result.empty();
      });
}
