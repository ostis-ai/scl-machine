/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "manager/InferenceManagerGeneral.hpp"

#include "searcher/TemplateSearcherAbstract.hpp"
#include "manager/TemplateManager.hpp"
#include "strategy/FormulasIterationStrategyAbstract.hpp"

namespace inference
{
class InferenceManagerBuilderAbstract
{
public:
  explicit InferenceManagerBuilderAbstract(ScMemoryContext * context);
  virtual ~InferenceManagerBuilderAbstract() = default;

  virtual InferenceManagerBuilderAbstract & setTemplateSearcher(std::unique_ptr<TemplateSearcherAbstract> templateSearcher) = 0;
  virtual InferenceManagerBuilderAbstract & setFormulasIterationStrategy(std::unique_ptr<FormulasIterationStrategyAbstract> strategy) = 0;

  InferenceManagerBuilderAbstract & setTemplateManager(std::unique_ptr<TemplateManagerAbstract> manager);
  std::unique_ptr<InferenceManagerGeneral> build();
  void reset();

protected:
  ScMemoryContext * context;

  std::unique_ptr<TemplateSearcherAbstract> templateSearcher;
  std::unique_ptr<TemplateManagerAbstract> templateManager;
  std::unique_ptr<InferenceManagerGeneral> inferenceManager;
};
}
