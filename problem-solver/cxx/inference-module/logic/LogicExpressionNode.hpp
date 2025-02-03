/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "utils/Types.hpp"
#include <memory>

namespace inference
{

struct LogicFormulaResult
{
  bool value = false;
  bool isGenerated = false;
  Replacements replacements{};
};

class LogicExpressionNode
{
public:
  LogicExpressionNode() = default;

  virtual void compute(LogicFormulaResult & result) const = 0;
  virtual ScAddr getFormula() const = 0;
  virtual ~LogicExpressionNode() = default;

  virtual void generate(Replacements & replacements, LogicFormulaResult & result) = 0;

  void setArgumentVector(ScAddrVector const & otherArgumentVector)
  {
    argumentVector = otherArgumentVector;
  }

  void setOutputStructureElements(std::unordered_set<ScAddr, ScAddrHashFunc> const & otherOutputStructureElements)
  {
    outputStructureElements = otherOutputStructureElements;
  }

protected:
  ScAddrVector argumentVector;
  std::unordered_set<ScAddr, ScAddrHashFunc> outputStructureElements;
};

class OperatorLogicExpressionNode : public LogicExpressionNode
{
public:
  using OperandsVector = std::vector<std::shared_ptr<LogicExpressionNode>>;

protected:
  OperandsVector operands;
};
}  // namespace inference
