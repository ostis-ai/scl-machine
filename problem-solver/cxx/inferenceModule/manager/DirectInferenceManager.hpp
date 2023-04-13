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

#include "manager/SolutionTreeManager.hpp"
#include "manager/TemplateManager.hpp"
#include "searcher/TemplateSearcher.hpp"
#include "classifier/FormulaClassifier.hpp"
#include "logic/LogicExpressionNode.hpp"

namespace inference
{
using ScAddrQueue = std::queue<ScAddr>;

// TODO: Fix issue with using interface InferenceManager
class DirectInferenceManager
{
public:
  explicit DirectInferenceManager(ScMemoryContext * ms_context);

  ScAddr applyInference(
      ScAddr const & targetStructure,
      ScAddr const & formulasSet,
      ScAddr const & arguments,
      ScAddr const & inputStructure);

private:
  std::vector<ScAddrQueue> createFormulasQueuesListByPriority(ScAddr const & formulasSet);

  ScAddrQueue createQueue(ScAddr const & set);

  LogicFormulaResult useFormula(ScAddr const & rule, ScAddrVector & argumentVector, ScAddr const & outputStructure);

  bool isTargetAchieved(ScAddr const & targetStructure, ScAddrVector const & argumentVector);

  ScMemoryContext * ms_context;
  std::shared_ptr<TemplateManagerAbstract> templateManager;
  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  std::shared_ptr<SolutionTreeManager> solutionTreeManager;
};
}  // namespace inference
