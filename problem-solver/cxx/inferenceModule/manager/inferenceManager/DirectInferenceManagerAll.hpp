/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "manager/templateManager/TemplateManager.hpp"
#include "logic/LogicExpressionNode.hpp"

#include "InferenceManagerAbstract.hpp"

namespace inference
{
using ScAddrQueue = std::queue<ScAddr>;

class DirectInferenceManagerAll : public InferenceManagerAbstract
{
public:
  explicit DirectInferenceManagerAll(ScMemoryContext * context);

  bool applyIterationStrategy(InferenceParamsConfig const & inferenceParamsConfig) override;
};
}
