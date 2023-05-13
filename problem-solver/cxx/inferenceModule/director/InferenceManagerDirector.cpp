/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceManagerDirector.hpp"

#include "searcher/TemplateSearcherInStructures.hpp"
#include "manager/TemplateManagerFixedArguments.hpp"
#include "strategy/FormulasIterationStrategyAll.hpp"
#include "strategy/FormulasIterationStrategyTarget.hpp"

using namespace inference;

std::unique_ptr<FormulasIterationStrategyAbstract> InferenceManagerDirector::
    constructInputStructuresFixedArgumentsIterationStrategyAll(
      ScMemoryContext * context, ScAddrVector const & inputStructures, ScAddrVector const & arguments)
{
  std::unique_ptr<FormulasIterationStrategyAll> strategyAll = std::make_unique<FormulasIterationStrategyAll>(context);
  strategyAll->setGenerateSolutionTree(false);

  std::shared_ptr<TemplateManagerAbstract> templateManager = std::make_shared<TemplateManagerFixedArguments>(context);
  templateManager->setGenerateOnlyFirst(false);
  templateManager->setGenerateOnlyUnique(false);
  templateManager->setArguments(arguments);
  strategyAll->setTemplateManager(templateManager);

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  if (inputStructures.empty())
  {
    templateSearcher = std::make_shared<TemplateSearcher>(context);
  }
  else
  {
    templateSearcher = std::make_shared<TemplateSearcherInStructures>(context);
    templateSearcher->setInputStructures(inputStructures);
  }

  strategyAll->setTemplateSearcher(templateSearcher);

  return strategyAll;
}

std::unique_ptr<FormulasIterationStrategyAbstract> InferenceManagerDirector::constructIterationStrategyTarget(
      ScMemoryContext * context, ScAddr const & targetStructure, ScAddrVector const & inputStructures, ScAddrVector const & arguments)
{
  std::unique_ptr<FormulasIterationStrategyTarget> strategyTarget = std::make_unique<FormulasIterationStrategyTarget>(context);
  strategyTarget->setTargetStructure(targetStructure);
  strategyTarget->setGenerateSolutionTree(false);

  std::shared_ptr<TemplateManagerAbstract> templateManager = std::make_shared<TemplateManager>(context);
  templateManager->setGenerateOnlyFirst(true);
  templateManager->setGenerateOnlyUnique(false);
  templateManager->setArguments(arguments);
  strategyTarget->setTemplateManager(templateManager);

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  if (inputStructures.empty())
  {
    templateSearcher = std::make_shared<TemplateSearcher>(context);
  }
  else
  {
    templateSearcher = std::make_shared<TemplateSearcherInStructures>(context);
    templateSearcher->setInputStructures(inputStructures);
  }
  strategyTarget->setTemplateSearcher(templateSearcher);

  return strategyTarget;
}
