/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "../keynodes/InferenceKeynodes.hpp"

namespace inference
{
class FormulaClassifier
{
public:
  enum FormulaClasses
  {
    NONE = 0,
    ATOMIC = 1,
    NEGATION = 2,
    CONJUNCTION = 3,
    DISJUNCTION = 4,
    IMPLICATION_EDGE = 5,
    IMPLICATION_TUPLE = 6,
    EQUIVALENCE_EDGE = 7,
    EQUIVALENCE_TUPLE = 8
  };

  static int typeOfFormula(ScMemoryContext * ms_context, ScAddr const & formula);
  static bool isFormulaWithConst(ScMemoryContext * ms_context, ScAddr const & formula);
  static bool isFormulaWithVar(ScMemoryContext * ms_context, ScAddr const & formula);
  static bool isFormulaToGenerate(ScMemoryContext * ms_context, ScAddr const & formula);
};

}  // namespace inference
