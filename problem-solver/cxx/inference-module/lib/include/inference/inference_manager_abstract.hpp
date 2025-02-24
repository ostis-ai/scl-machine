/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>

#include "inference/inference_config.hpp"
#include "inference/solution_tree_manager_abstract.hpp"

namespace inference
{
class TemplateManagerAbstract;
class TemplateSearcherAbstract;
class LogicFormulaResult;

using ScAddrQueue = std::queue<ScAddr>;

/// Class to control inference flow: when to stop applying rules and how to iterate over them
class InferenceManagerAbstract
{
public:
  explicit InferenceManagerAbstract(ScMemoryContext * context);

  virtual ~InferenceManagerAbstract();

  void SetTemplateSearcher(std::shared_ptr<TemplateSearcherAbstract> searcher);
  void SetTemplateManager(std::shared_ptr<TemplateManagerAbstract> manager);
  void SetSolutionTreeManager(std::shared_ptr<SolutionTreeManagerAbstract> manager);

  std::shared_ptr<SolutionTreeManagerAbstract> GetSolutionTreeManager();

  /**
   * @brief Iterate over formulas set and use formulas to generate knowledge
   * @param formulasSet is an oriented set of formulas sets to apply
   * @param outputStructure is a structure to write in generated constructions
   * @returns true if something was generated (any rule was applied), otherwise return false
   * @throws utils::ExceptionItemNotFound Thrown if `formulasSet` is an empty set
   */
  virtual bool ApplyInference(InferenceParams const & inferenceParamsConfig) = 0;

  // TODO: Need to implement common logic of inference rules (e.g. modus ponens)
  LogicFormulaResult UseFormula(ScAddr const & formula, ScAddr const & outputStructure);

  void FillFormulaFixedArgumentsIdentifiers(ScAddr const & formula, ScAddr const & firstFixedArgument) const;

  void FormTemplateManagerFixedArguments(ScAddr const & formula, ScAddr const & firstFixedArgument);
  void ResetTemplateManager(std::shared_ptr<TemplateManagerAbstract> otherTemplateManager);

  std::vector<ScAddrQueue> CreateFormulasQueuesListByPriority(ScAddr const & formulasSet);

  ScAddrQueue CreateQueue(ScAddr const & set);

protected:
  ScMemoryContext * context;

  std::shared_ptr<TemplateManagerAbstract> templateManager;
  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;

  std::unordered_set<ScAddr, ScAddrHashFunc> outputStructureElements;
};
}  // namespace inference
