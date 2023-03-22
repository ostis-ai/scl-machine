/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceManagerDirector.hpp"

#include "searcher/TemplateSearcherInStructures.hpp"
#include "strategy/FormulasIterationStrategyAll.hpp"
#include "strategy/FormulasIterationStrategyTarget.hpp"

using namespace inference;

std::unique_ptr<InferenceManagerGeneral> InferenceManagerDirector::constructDirectInferenceManagerInputStructuresStrategyAll(
      ScMemoryContext * context, std::unique_ptr<InferenceManagerBuilderAbstract> builder)
{
  return builder
    ->setTemplateSearcher(std::make_unique<TemplateSearcherInStructures>(context))
    .setFormulasIterationStrategy(std::make_unique<FormulasIterationStrategyAll>(context))
    .build();
}

std::unique_ptr<InferenceManagerGeneral> InferenceManagerDirector::constructDirectInferenceManagerStrategyTarget(
      ScMemoryContext * context, std::unique_ptr<InferenceManagerBuilderAbstract> builder)
{
  return builder
    ->setTemplateSearcher(std::make_unique<TemplateSearcher>(context))
    .setFormulasIterationStrategy(std::make_unique<FormulasIterationStrategyTarget>(context))
    .build();
}
