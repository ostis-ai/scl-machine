/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/kpm/sc_agent.hpp>

#include "model/SolutionTreeNode.hpp"

namespace inference
{

// TODO: Fix issue with using interface InferenceManager
class DirectInferenceManager
{
public:
  explicit DirectInferenceManager(ScMemoryContext * ms_context);

  SolutionTreeNode & applyInference(
        const ScAddr & targetTemplate,
        const ScAddr & ruleSet,
        const ScAddr & argumentSet);

  ~DirectInferenceManager() = default;

private:
  ScMemoryContext * ms_context;
};



}
