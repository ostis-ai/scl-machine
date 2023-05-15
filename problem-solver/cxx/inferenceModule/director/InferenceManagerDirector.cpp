/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceManagerDirector.hpp"

#include "searcher/TemplateSearcherInStructures.hpp"
#include "manager/TemplateManagerFixedArguments.hpp"
#include "manager/SolutionTreeManagerEmpty.hpp"
#include "manager/SolutionTreeManager.hpp"
#include "strategy/FormulasIterationStrategyAll.hpp"
#include "strategy/FormulasIterationStrategyTarget.hpp"

using namespace inference;

std::unique_ptr<FormulasIterationStrategyAbstract> InferenceManagerDirector::constructDirectInferenceManagerAll(
      ScMemoryContext * context, InferenceFlowConfig const & inferenceFlowConfig, ScAddrVector const & inputStructures)
{
  std::unique_ptr<FormulasIterationStrategyAll> strategyAll = std::make_unique<FormulasIterationStrategyAll>(context);
  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;
  if (inferenceFlowConfig.generateSolutionTree)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManager>(context);
  }
  else
  {
    solutionTreeManager = std::make_unique<SolutionTreeManagerEmpty>(context);
  }
  strategyAll->setSolutionTreeManager(solutionTreeManager);

  std::shared_ptr<TemplateManagerAbstract> templateManager = std::make_shared<TemplateManagerFixedArguments>(context);
  templateManager->setGenerateOnlyFirst(inferenceFlowConfig.generateOnlyFirst);
  templateManager->setGenerateOnlyUnique(inferenceFlowConfig.generateOnlyUnique);
  strategyAll->setTemplateManager(templateManager);

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  if (inputStructures.empty())
  {
    templateSearcher = std::make_shared<TemplateSearcher>(context);
  }
  else
  {
    templateSearcher = std::make_shared<TemplateSearcherInStructures>(context);
  }
  strategyAll->setTemplateSearcher(templateSearcher);

  return strategyAll;
}

std::unique_ptr<FormulasIterationStrategyAbstract> InferenceManagerDirector::constructDirectInferenceManagerTarget(
      ScMemoryContext * context, InferenceFlowConfig const & inferenceFlowConfig, ScAddrVector const & inputStructures)
{
  std::unique_ptr<FormulasIterationStrategyTarget> strategyTarget = std::make_unique<FormulasIterationStrategyTarget>(context);

  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;
  if (inferenceFlowConfig.generateSolutionTree)
  {
    solutionTreeManager = std::make_unique<SolutionTreeManager>(context);
  }
  else
  {
    solutionTreeManager = std::make_unique<SolutionTreeManagerEmpty>(context);
  }
  strategyTarget->setSolutionTreeManager(solutionTreeManager);

  std::shared_ptr<TemplateManagerAbstract> templateManager = std::make_shared<TemplateManager>(context);
  templateManager->setGenerateOnlyFirst(inferenceFlowConfig.generateOnlyFirst);
  templateManager->setGenerateOnlyUnique(inferenceFlowConfig.generateOnlyUnique);
  strategyTarget->setTemplateManager(templateManager);

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  if (inputStructures.empty())
  {
    templateSearcher = std::make_shared<TemplateSearcher>(context);
  }
  else
  {
    templateSearcher = std::make_shared<TemplateSearcherInStructures>(context);
  }
  strategyTarget->setTemplateSearcher(templateSearcher);

  return strategyTarget;
}
