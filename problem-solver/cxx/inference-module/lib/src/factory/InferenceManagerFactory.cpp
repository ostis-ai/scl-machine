/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "inference/inference_manager_factory.hpp"

#include "searcher/template-searcher/TemplateSearcherOnlyMembershipArcsInStructures.hpp"
#include "searcher/template-searcher/TemplateSearcherInStructures.hpp"
#include "searcher/template-searcher/TemplateSearcherGeneral.hpp"
#include "manager/template-manager/TemplateManagerFixedArguments.hpp"
#include "manager/solution-tree-manager/SolutionTreeManagerEmpty.hpp"
#include "manager/solution-tree-manager/SolutionTreeManager.hpp"
#include "manager/inference-manager/DirectInferenceManagerAll.hpp"
#include "manager/inference-manager/DirectInferenceManagerTarget.hpp"

using namespace inference;

std::unique_ptr<InferenceManagerAbstract> InferenceManagerFactory::ConstructDirectInferenceManagerAll(
    ScMemoryContext * context,
    utils::ScLogger * logger,
    InferenceConfig const & inferenceFlowConfig)
{
  std::unique_ptr<DirectInferenceManagerAll> strategyAll = std::make_unique<DirectInferenceManagerAll>(context, logger);
  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;
  if (inferenceFlowConfig.solutionTreeType == TREE_FULL)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManager>(context);
  }
  else if (inferenceFlowConfig.solutionTreeType == TREE_ONLY_OUTPUT_STRUCTURE)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManagerEmpty>(context);
  }
  strategyAll->SetSolutionTreeManager(solutionTreeManager);

  std::shared_ptr<TemplateManagerAbstract> templateManager = std::make_shared<TemplateManagerFixedArguments>(context);
  templateManager->SetReplacementsUsingType(inferenceFlowConfig.replacementsUsingType);
  templateManager->SetGenerationType(inferenceFlowConfig.generationType);
  templateManager->SetFillingType(inferenceFlowConfig.fillingType);
  strategyAll->SetTemplateManager(templateManager);

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  if (inferenceFlowConfig.searchType == SEARCH_IN_ALL_KB)
  {
    templateSearcher = std::make_shared<TemplateSearcherGeneral>(context);
  }
  else if (inferenceFlowConfig.searchType == SEARCH_IN_STRUCTURES)
  {
    templateSearcher = std::make_shared<TemplateSearcherInStructures>(context);
  }
  else if (inferenceFlowConfig.searchType == SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES)
  {
    templateSearcher = std::make_shared<TemplateSearcherOnlyMembershipArcsInStructures>(context);
  }
  templateSearcher->SetReplacementsUsingType(inferenceFlowConfig.replacementsUsingType);
  templateSearcher->setOutputStructureFillingType(inferenceFlowConfig.fillingType);
  templateSearcher->setAtomicLogicalFormulaSearchBeforeGenerationType(
      inferenceFlowConfig.atomicLogicalFormulaSearchBeforeGenerationType);
  strategyAll->SetTemplateSearcher(templateSearcher);

  return strategyAll;
}

std::unique_ptr<InferenceManagerAbstract> InferenceManagerFactory::ConstructDirectInferenceManagerTarget(
    ScMemoryContext * context,
    utils::ScLogger * logger,
    InferenceConfig const & inferenceFlowConfig)
{
  std::unique_ptr<DirectInferenceManagerTarget> strategyTarget =
      std::make_unique<DirectInferenceManagerTarget>(context, logger);

  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;
  if (inferenceFlowConfig.solutionTreeType == TREE_FULL)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManager>(context);
  }
  else if (inferenceFlowConfig.solutionTreeType == TREE_ONLY_OUTPUT_STRUCTURE)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManagerEmpty>(context);
  }
  strategyTarget->SetSolutionTreeManager(solutionTreeManager);

  std::shared_ptr<TemplateManagerAbstract> templateManager = std::make_shared<TemplateManager>(context);
  templateManager->SetReplacementsUsingType(inferenceFlowConfig.replacementsUsingType);
  templateManager->SetGenerationType(inferenceFlowConfig.generationType);
  templateManager->SetFillingType(inferenceFlowConfig.fillingType);
  strategyTarget->SetTemplateManager(templateManager);

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  if (inferenceFlowConfig.searchType == SEARCH_IN_ALL_KB)
  {
    templateSearcher = std::make_shared<TemplateSearcherGeneral>(context);
  }
  else if (inferenceFlowConfig.searchType == SEARCH_IN_STRUCTURES)
  {
    templateSearcher = std::make_shared<TemplateSearcherInStructures>(context);
  }
  else if (inferenceFlowConfig.searchType == SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES)
  {
    templateSearcher = std::make_shared<TemplateSearcherOnlyMembershipArcsInStructures>(context);
  }
  templateSearcher->SetReplacementsUsingType(inferenceFlowConfig.replacementsUsingType);
  templateSearcher->setOutputStructureFillingType(inferenceFlowConfig.fillingType);
  templateSearcher->setAtomicLogicalFormulaSearchBeforeGenerationType(
      inferenceFlowConfig.atomicLogicalFormulaSearchBeforeGenerationType);
  strategyTarget->SetTemplateSearcher(templateSearcher);

  return strategyTarget;
}
