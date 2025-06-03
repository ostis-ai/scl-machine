/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DirectInferenceManagerAll.hpp"

#include "inference/inference_keynodes.hpp"

#include "logic/LogicExpressionNode.hpp"

#include "inference/solution_tree_manager_abstract.hpp"
#include "inference/template_manager_abstract.hpp"
#include "inference/replacements_utils.hpp"

#include "searcher/template-searcher/TemplateSearcherAbstract.hpp"

using namespace inference;

DirectInferenceManagerAll::DirectInferenceManagerAll(ScMemoryContext * context, utils::ScLogger * logger)
  : InferenceManagerAbstract(context, logger), logger(logger)
{
}

bool DirectInferenceManagerAll::ApplyInference(InferenceParams const & inferenceParamsConfig)
{
  bool result = false;

  templateManager->SetArguments(inferenceParamsConfig.arguments);
  templateSearcher->setInputStructures(inferenceParamsConfig.inputStructures);

  std::vector<ScAddrQueue> formulasQueuesByPriority =
      CreateFormulasQueuesListByPriority(inferenceParamsConfig.formulasSet);
  if (formulasQueuesByPriority.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No formulas sets found.");
  }

  ScAddrQueue uncheckedFormulas;
  ScAddr formula;
  LogicFormulaResult formulaResult;
  logger->Debug("Start formulas applying. There is ", formulasQueuesByPriority.size(), " formulas sets");
  for (size_t formulasQueueIndex = 0; formulasQueueIndex < formulasQueuesByPriority.size(); formulasQueueIndex++)
  {
    uncheckedFormulas = formulasQueuesByPriority[formulasQueueIndex];
    logger->Debug("There is ", uncheckedFormulas.size(), " formulas in ", (formulasQueueIndex + 1), " set");
    while (!uncheckedFormulas.empty())
    {
      formula = uncheckedFormulas.front();
      logger->Debug("Trying to generate by formula: ", context->GetElementSystemIdentifier(formula));
      formulaResult = UseFormula(formula, inferenceParamsConfig.outputStructure);
      logger->Debug("Logical formula is ", (formulaResult.isGenerated ? "generated" : "not generated"));
      if (formulaResult.isGenerated)
      {
        result = true;
        solutionTreeManager->AddNode(formula, formulaResult.replacements);
      }

      uncheckedFormulas.pop();
    }
  }
  formulaResult.replacements.clear();
  return result;
}
