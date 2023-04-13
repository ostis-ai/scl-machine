/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "utils/ReplacementsUtils.hpp"

struct LogicExpressionResult
{
  bool value = false;
  bool hasSearchResult = false;
  Replacements replacements{};
  ScAddr formulaTemplate{};
};

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

  virtual LogicExpressionResult check(ScTemplateParams & params) const = 0;
  virtual LogicFormulaResult compute(LogicFormulaResult & result) const = 0;
  virtual ScAddr getFormulaTemplate() const = 0;
  virtual ~LogicExpressionNode() = default;

  virtual LogicFormulaResult generate(Replacements & replacements) const = 0;

  void setArgumentVector(ScAddrVector const & otherArgumentVector)
  {
    argumentVector = otherArgumentVector;
  }

protected:
  ScAddrVector argumentVector;
};

class OperatorLogicExpressionNode : public LogicExpressionNode
{
public:
  using OperandsVector = std::vector<std::shared_ptr<LogicExpressionNode>>;

protected:
  OperandsVector operands;
};
