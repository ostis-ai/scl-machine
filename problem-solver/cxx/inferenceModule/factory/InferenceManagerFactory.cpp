/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceManagerFactory.hpp"

#include "searcher/templateSearcher/TemplateSearcherInStructures.hpp"
#include "searcher/templateSearcher/TemplateSearcherGeneral.hpp"
#include "manager/templateManager/TemplateManagerFixedArguments.hpp"
#include "manager/solutionTreeManager/SolutionTreeManagerEmpty.hpp"
#include "manager/solutionTreeManager/SolutionTreeManager.hpp"
#include "manager/inferenceManager/DirectInferenceManagerAll.hpp"
#include "manager/inferenceManager/DirectInferenceManagerTarget.hpp"

using namespace inference;

std::unique_ptr<InferenceManagerAbstract> InferenceManagerFactory::constructDirectInferenceManagerAll(
    ScMemoryContext * context,
    InferenceConfig const & inferenceFlowConfig)
{
  std::unique_ptr<DirectInferenceManagerAll> strategyAll = std::make_unique<DirectInferenceManagerAll>(context);
  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;
  if (inferenceFlowConfig.solutionTreeType == TREE_FULL)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManager>(context);
  }
  else if (inferenceFlowConfig.solutionTreeType == TREE_ONLY_OUTPUT_STRUCTURE)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManagerEmpty>(context);
  }
  strategyAll->setSolutionTreeManager(solutionTreeManager);

  std::shared_ptr<TemplateManagerAbstract> templateManager = std::make_shared<TemplateManagerFixedArguments>(context);
  templateManager->setReplacementsUsingType(inferenceFlowConfig.replacementsUsingType);
  templateManager->setGenerationType(inferenceFlowConfig.generationType);
  strategyAll->setTemplateManager(templateManager);

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  if (inferenceFlowConfig.searchType == SEARCH_IN_ALL_KB)
  {
    templateSearcher = std::make_shared<TemplateSearcherGeneral>(context);
  }
  else if (inferenceFlowConfig.searchType == SEARCH_IN_STRUCTURES)
  {
    templateSearcher = std::make_shared<TemplateSearcherInStructures>(context);
  }
  strategyAll->setTemplateSearcher(templateSearcher);

  return strategyAll;
}

std::unique_ptr<InferenceManagerAbstract> InferenceManagerFactory::constructDirectInferenceManagerTarget(
    ScMemoryContext * context,
    InferenceConfig const & inferenceFlowConfig)
{
  std::unique_ptr<DirectInferenceManagerTarget> strategyTarget =
      std::make_unique<DirectInferenceManagerTarget>(context);

  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;
  if (inferenceFlowConfig.solutionTreeType == TREE_FULL)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManager>(context);
  }
  else if (inferenceFlowConfig.solutionTreeType == TREE_ONLY_OUTPUT_STRUCTURE)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManagerEmpty>(context);
  }
  strategyTarget->setSolutionTreeManager(solutionTreeManager);

  std::shared_ptr<TemplateManagerAbstract> templateManager = std::make_shared<TemplateManager>(context);
  templateManager->setReplacementsUsingType(inferenceFlowConfig.replacementsUsingType);
  templateManager->setGenerationType(inferenceFlowConfig.generationType);
  strategyTarget->setTemplateManager(templateManager);

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  if (inferenceFlowConfig.searchType == SEARCH_IN_ALL_KB)
  {
    templateSearcher = std::make_shared<TemplateSearcherGeneral>(context);
  }
  else if (inferenceFlowConfig.searchType == SEARCH_IN_STRUCTURES)
  {
    templateSearcher = std::make_shared<TemplateSearcherInStructures>(context);
  }
  strategyTarget->setTemplateSearcher(templateSearcher);

  return strategyTarget;
}
