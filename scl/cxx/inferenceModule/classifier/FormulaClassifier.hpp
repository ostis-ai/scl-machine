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
  FormulaClassifier(ScMemoryContext * ms_context);
  static int NONE;
  static int ATOM;
  static int NEGATION;
  static int CONJUNCTION;
  static int DISJUNCTION;
  static int IMPLICATION_EDGE;
  static int IMPLICATION_TUPLE;
  static int EQUIVALENCE_EDGE;
  static int EQUIVALENCE_TUPLE;

  int typeOfFormula(ScAddr const & formula);
  bool isFormulaWithConst(ScAddr const & formula);
  bool isFormulaToGenerate(ScAddr const & formula);

private:
  ScMemoryContext * ms_context;
};

}  // namespace inference
