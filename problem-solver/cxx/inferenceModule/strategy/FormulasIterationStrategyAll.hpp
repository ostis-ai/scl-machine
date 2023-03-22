/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "manager/TemplateManager.hpp"
#include "logic/LogicExpressionNode.hpp"

#include "FormulasIterationStrategyAbstract.hpp"

namespace inference
{
using ScAddrQueue = std::queue<ScAddr>;

// Или FormulasIterationStrategyFull то есть перебираем все формулы, генерим все, что можно. С названием туго
class FormulasIterationStrategyAll : public FormulasIterationStrategyAbstract
{
public:
  explicit FormulasIterationStrategyAll(ScMemoryContext * context);

  bool applyIterationStrategy(ScAddr const & formulasSet, ScAddr const & outputStructure) override;

protected:
  std::unique_ptr<TemplateManager> templateManager;

  ScAddr arguments;
};
}
