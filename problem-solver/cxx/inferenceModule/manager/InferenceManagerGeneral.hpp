/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <queue>
#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

#include "searcher/TemplateSearcher.hpp"
#include "classifier/FormulaClassifier.hpp"
#include "logic/LogicExpressionNode.hpp"

#include "strategy/FormulasIterationStrategyAbstract.hpp"
#include "SolutionTreeManager.hpp"
#include "TemplateManager.hpp"

namespace inference
{
using ScAddrQueue = std::queue<ScAddr>;

class InferenceManagerGeneral
{
public:
  explicit InferenceManagerGeneral(ScMemoryContext * ms_context);

  ~InferenceManagerGeneral() = default;

  ScAddr applyInference(ScAddr const & formulasSet);

  void setFormulasIterationStrategy(std::unique_ptr<FormulasIterationStrategyAbstract> strategy);

protected:
  ScMemoryContext * ms_context;

  std::unique_ptr<FormulasIterationStrategyAbstract> formulasIterationStrategy;
};
}  // namespace inference
