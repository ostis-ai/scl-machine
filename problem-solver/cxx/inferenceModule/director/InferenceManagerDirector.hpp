/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "manager/InferenceManagerGeneral.hpp"
#include "builder/InferenceManagerBuilderAbstract.hpp"

namespace inference
{
class InferenceManagerDirector
{
public:
  static std::unique_ptr<InferenceManagerGeneral> constructDirectInferenceManagerInputStructuresStrategyAll(
        ScMemoryContext * context, std::unique_ptr<InferenceManagerBuilderAbstract> builder);

  static std::unique_ptr<InferenceManagerGeneral> constructDirectInferenceManagerStrategyTarget(
        ScMemoryContext * context, std::unique_ptr<InferenceManagerBuilderAbstract> builder);
};
}
