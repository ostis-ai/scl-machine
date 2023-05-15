/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "InferenceManagerAbstract.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "logic/LogicExpressionNode.hpp"

namespace inference
{
/// Formulas iteration strategy stops iteration if target is achieved
class DirectInferenceManagerTarget : public InferenceManagerAbstract
{
public:
  explicit DirectInferenceManagerTarget(ScMemoryContext * context);

  bool applyIterationStrategy(InferenceParamsConfig const & inferenceParamsConfig) override;

  void setTargetStructure(ScAddr const & otherTargetStructure);

  bool isTargetAchieved(std::vector<ScTemplateParams> const & templateParamsVector);

protected:
  ScAddr targetStructure;
};
}
