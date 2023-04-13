/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceManagerInputStructuresBuilder.hpp"
#include "searcher/TemplateSearcherInStructures.hpp"
#include "strategy/FormulasIterationStrategyAll.hpp"

using namespace inference;

InferenceManagerInputStructuresBuilder::InferenceManagerInputStructuresBuilder(
      ScMemoryContext * context, ScAddr const & otherInputStructures, ScAddr const & otherArguments) : InferenceManagerBuilderAbstract(context)
{
  inputStructures = otherInputStructures;
  arguments = otherArguments;
}

InferenceManagerBuilderAbstract & InferenceManagerInputStructuresBuilder::setTemplateSearcher(std::unique_ptr<TemplateSearcherAbstract> searcher)
{
  templateSearcher = std::move(searcher);
  templateSearcher->setInputStructures(inputStructures);

  return *this;
}

InferenceManagerBuilderAbstract & InferenceManagerInputStructuresBuilder::setFormulasIterationStrategy(std::unique_ptr<FormulasIterationStrategyAbstract> strategy)
{
  templateManager->setGenerateOnlyFirst(false);
  templateManager->setGenerateOnlyUnique(false);

  strategy->setTemplateSearcher(std::move(templateSearcher));
  strategy->setTemplateManager(std::move(templateManager));
  strategy->setArguments(arguments);

  inferenceManager->setFormulasIterationStrategy(std::move(strategy));

  return *this;
}
