/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_template.hpp>

#include <keynodes/InferenceKeynodes.hpp>
#include <manager/TemplateManager.hpp>
#include "searcher/TemplateSearcher.hpp"

using namespace inference;

struct LogicExpressionResult
{
  bool value;
  bool hasSearchResult;
  ScTemplateSearchResultItem templateSearchResult{nullptr, nullptr};
  ScAddr formulaTemplate{};
};

class LogicExpressionNode
{
public:
  virtual LogicExpressionResult check(ScTemplateParams params) const = 0;
};

class OperatorLogicExpressionNode : public LogicExpressionNode
{
public:
  using OperandsVectorType = std::vector<std::unique_ptr<LogicExpressionNode>>;

protected:
  OperandsVectorType operands;
};

class AndExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit AndExpressionNode(OperandsVectorType & operands);

  LogicExpressionResult check(ScTemplateParams params) const override;
};

class OrExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit OrExpressionNode(OperandsVectorType & operands);

  LogicExpressionResult check(ScTemplateParams params) const override;
};

class NotExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit NotExpressionNode(std::unique_ptr<LogicExpressionNode> op);

  LogicExpressionResult check(ScTemplateParams params) const override;
};

class TemplateExpressionNode : public LogicExpressionNode
{
public:
  TemplateExpressionNode(ScMemoryContext * context, ScAddr formulaTemplate, TemplateSearcher * templateSearcher);

  LogicExpressionResult check(ScTemplateParams params) const override;

private:
  ScMemoryContext * context;
  ScAddr formulaTemplate;
  inference::TemplateSearcher * templateSearcher;
};

class LogicExpression
{
public:
  LogicExpression(
      ScMemoryContext * context,
      TemplateSearcher * templateSearcher,
      TemplateManager * templateManager,
      ScAddrVector argumentList
  );

  std::unique_ptr<LogicExpressionNode> build(ScAddr const & node);

  std::vector<ScTemplateParams> const & GetParamsSet() const
  {
    return paramsSet;
  }

private:
  ScMemoryContext * context;
  std::vector<ScTemplateParams> paramsSet;

  TemplateSearcher * templateSearcher;
  TemplateManager  * templateManager;
  ScAddrVector argumentList;
};
