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

#include "manager/solutionTreeManager/SolutionTreeManager.hpp"
#include "manager/templateManager/TemplateManager.hpp"
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
  ScMemoryContext * ms_context;
};
}  // namespace inference
