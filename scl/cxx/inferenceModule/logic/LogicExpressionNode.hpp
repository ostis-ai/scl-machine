/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "utils/ReplacementsUtils.hpp"

struct LogicExpressionResult
{
  bool value;
  bool hasSearchResult;
  ScTemplateSearchResultItem templateSearchResult{nullptr, nullptr};
  ScAddr formulaTemplate{};
};

struct LogicFormulaResult
{
  bool value;
  bool isGenerated;
  Replacements replacements;
};

class LogicExpressionNode
{
public:
  virtual LogicExpressionResult check(ScTemplateParams & params) const = 0;
  virtual LogicFormulaResult compute(LogicFormulaResult & result) const = 0;

  virtual ScAddr getFormulaTemplate() const = 0;
};

class OperatorLogicExpressionNode : public LogicExpressionNode
{
public:
  using OperandsVector = std::vector<std::unique_ptr<LogicExpressionNode>>;

protected:
  OperandsVector operands;
};
