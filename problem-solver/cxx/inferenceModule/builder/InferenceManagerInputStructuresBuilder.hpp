/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "InferenceManagerBuilderAbstract.hpp"

namespace inference
{
class InferenceManagerInputStructuresBuilder : public InferenceManagerBuilderAbstract
{
public:
  explicit InferenceManagerInputStructuresBuilder(ScMemoryContext * context, ScAddrVector const & otherInputStructures, ScAddrVector const & otherArguments = {});

  InferenceManagerBuilderAbstract & setTemplateSearcher(std::unique_ptr<TemplateSearcherAbstract> searcher) override;
  InferenceManagerBuilderAbstract & setFormulasIterationStrategy(std::unique_ptr<FormulasIterationStrategyAbstract> strategy) override;

protected:
  ScAddrVector inputStructures;
  ScAddrVector arguments;
};
}
