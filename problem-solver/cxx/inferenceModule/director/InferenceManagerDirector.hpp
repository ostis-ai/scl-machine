/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "strategy/FormulasIterationStrategyAbstract.hpp"

namespace inference
{
class InferenceManagerDirector
{
public:
  static std::unique_ptr<FormulasIterationStrategyAbstract> constructInputStructuresFixedArgumentsIterationStrategyAll(
        ScMemoryContext * context, ScAddrVector const & inputStructures = {}, ScAddrVector const & arguments = {});

  static std::unique_ptr<FormulasIterationStrategyAbstract> constructIterationStrategyTarget(
        ScMemoryContext * context, ScAddrVector const & inputStructures = {}, ScAddrVector const & arguments = {});
};
}
