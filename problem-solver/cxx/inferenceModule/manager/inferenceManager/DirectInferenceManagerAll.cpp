/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DirectInferenceManagerAll.hpp"

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

DirectInferenceManagerAll::DirectInferenceManagerAll(ScMemoryContext * context)
  : InferenceManagerAbstract(context)
{
}

bool DirectInferenceManagerAll::applyInference(InferenceParams const & inferenceParamsConfig)
{
  bool result = false;

  templateManager->setArguments(inferenceParamsConfig.arguments);
  templateSearcher->setInputStructures(inferenceParamsConfig.inputStructures);

  vector<ScAddrQueue> formulasQueuesByPriority = createFormulasQueuesListByPriority(inferenceParamsConfig.formulasSet);
  if (formulasQueuesByPriority.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No formulas sets found.");
  }

  ScAddrQueue uncheckedFormulas;
  ScAddr formula;
  LogicFormulaResult formulaResult;
  SC_LOG_DEBUG("Start formulas applying. There is " << formulasQueuesByPriority.size() << " formulas sets");
  for (size_t formulasQueueIndex = 0; formulasQueueIndex < formulasQueuesByPriority.size(); formulasQueueIndex++)
  {
    uncheckedFormulas = formulasQueuesByPriority[formulasQueueIndex];
    SC_LOG_DEBUG("There is " << uncheckedFormulas.size() << " formulas in " << (formulasQueueIndex + 1) << " set");
    while (!uncheckedFormulas.empty())
    {
      formula = uncheckedFormulas.front();
      SC_LOG_DEBUG("Trying to generate by formula: " << context->HelperGetSystemIdtf(formula));
      formulaResult = useFormula(formula, inferenceParamsConfig.outputStructure);
      SC_LOG_DEBUG("Logical formula is " << (formulaResult.isGenerated ? "generated" : "not generated"));
      if (formulaResult.isGenerated)
      {
        result = true;
        solutionTreeManager->addNode(formula, formulaResult.replacements);
      }

      uncheckedFormulas.pop();
    }
  }

  return result;
}
