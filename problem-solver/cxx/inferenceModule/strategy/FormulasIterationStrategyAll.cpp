/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "FormulasIterationStrategyAll.hpp"

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

FormulasIterationStrategyAll::FormulasIterationStrategyAll(ScMemoryContext * context)
      : FormulasIterationStrategyAbstract(context)
{
}

/**
 * @brief Iterate over formulas set and use formulas to generate knowledge
 *
 * Use all formulas for all suitable knowledge base constructions.
 * Don't stop at first success applying. Don't reiterate if something was generated
 *
 * @param formulasSet is an oriented set of formulas sets to apply
 * @param outputStructure is a structure to write in generated constructions
 * @returns true if something was generated (any rule was applied), otherwise return false
 * @throws utils::ExceptionItemNotFound Thrown if `formulasSet` is an empty set
*/
bool FormulasIterationStrategyAll::applyIterationStrategy(ScAddr const & formulasSet, ScAddr const & outputStructure)
{
  bool result = false;
  templateSearcher->setArguments(arguments);

  vector<ScAddrQueue> formulasQueuesByPriority = createFormulasQueuesListByPriority(formulasSet);
  if (formulasQueuesByPriority.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No rule sets found.");
  }

  ScAddrQueue uncheckedFormulas;
  ScAddr formula;
  LogicFormulaResult formulaResult;
  SC_LOG_DEBUG("Start rule applying. There is " << formulasQueuesByPriority.size() << " formulas sets");
  for (size_t formulasQueueIndex = 0; formulasQueueIndex < formulasQueuesByPriority.size(); formulasQueueIndex++)
  {
    uncheckedFormulas = formulasQueuesByPriority[formulasQueueIndex];
    SC_LOG_DEBUG("There is " << uncheckedFormulas.size() << " formulas in " << (formulasQueueIndex + 1) << " set");
    while (!uncheckedFormulas.empty())
    {
      formula = uncheckedFormulas.front();
      SC_LOG_DEBUG("Trying to generate by formula: " << context->HelperGetSystemIdtf(formula));
      formulaResult = useFormula(formula, arguments, outputStructure);
      SC_LOG_DEBUG("Logical formula is " << (formulaResult.isGenerated ? "generated" : "not generated"));
      if (formulaResult.isGenerated)
      {
        result = true;
        if (generateSolutionTree)
        {
          std::set<std::string> varNames;
          ReplacementsUtils::getKeySet(formulaResult.replacements, varNames);
          solutionTreeManager->addNode(
              formula,
              ReplacementsUtils::getReplacementsToScTemplateParams(formulaResult.replacements),
              varNames);
        }
      }

      uncheckedFormulas.pop();
    }
  }

  return result;
}
