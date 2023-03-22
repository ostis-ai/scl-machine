/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "FormulasIterationStrategyAll.hpp"

using namespace inference;

FormulasIterationStrategyAll::FormulasIterationStrategyAll(ScMemoryContext * context)
      : FormulasIterationStrategyAbstract(context)
{
}

bool FormulasIterationStrategyAll::applyIterationStrategy(ScAddr const & formulasSet, ScAddr const & outputStructure)
{
  // Реализация стратегии обхода всех формул без шаблона цели
  return true;
}
