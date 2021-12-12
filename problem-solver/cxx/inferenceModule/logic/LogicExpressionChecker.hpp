/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <utility>
#include <vector>
#include <sc-memory/sc_template.hpp>
#include <manager/TemplateManager.hpp>
#include "searcher/TemplateSearcher.hpp"

struct LogicExpressionResult
{
  bool result;
  bool hasTemplateSearchResult;
  ScTemplateSearchResultItem templateSearchResult{nullptr, nullptr};
  ScAddr templateItself{};
};

class LogicExpressionNode
{
public:
  virtual LogicExpressionResult check(ScTemplateParams params) const = 0;

  virtual std::string toString() const = 0;
};

class OperatorLogicExpressionNode : public LogicExpressionNode
{
public:
  using OperandsVectorType = std::vector<std::unique_ptr<LogicExpressionNode>>;

protected:
  std::string toString(std::string operatorName) const;

  void checkOperands(bool & areThereTrue, bool & areThereFalse,
                     ScAddr& templateItself,
                     ScTemplateSearchResultItem& searchResult,
                     bool & isThereSearchResult,
                     const ScTemplateParams& params) const;

  OperandsVectorType operands;
};

class AndExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit AndExpressionNode(OperandsVectorType & operands);

  LogicExpressionResult check(ScTemplateParams params) const override;

  std::string toString() const override
  {
    return OperatorLogicExpressionNode::toString("AND");
  }
};

class OrExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit OrExpressionNode(OperandsVectorType & operands);

  LogicExpressionResult check(ScTemplateParams params) const override;

  std::string toString() const override
  {
    return OperatorLogicExpressionNode::toString("OR");
  }
};

class NotExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit NotExpressionNode(std::unique_ptr<LogicExpressionNode> op);

  LogicExpressionResult check(ScTemplateParams params) const override;

  std::string toString() const override
  {
    return OperatorLogicExpressionNode::toString("NOT");
  }
};

class TemplateExpressionNode : public LogicExpressionNode
{
public:
  TemplateExpressionNode(ScAddr templateToCheck,
                         inference::TemplateSearcher * templateSearcher,
                         ScMemoryContext * context);

  LogicExpressionResult check(ScTemplateParams params) const override;

  std::string toString() const override
  {
    std::string sysIdtf = context->HelperGetSystemIdtf(templateToCheck);

    return sysIdtf.empty() ? std::to_string(templateToCheck.GetRealAddr().seg) + ":" +
                             std::to_string(templateToCheck.GetRealAddr().offset) : sysIdtf;
  }

private:
  ScAddr templateToCheck;
  inference::TemplateSearcher * templateSearcher;
  ScMemoryContext * context;
};

class LogicExpression
{
public:
  LogicExpression(ScAddr condition, ScMemoryContext * context,
                  inference::TemplateSearcher * templateSearcher,
                  inference::TemplateManager * templateManager,
                  ScAddrVector argumentList);

  void build();

  std::string toString()
  {
    return root->toString();
  }

  std::unique_ptr<LogicExpressionNode> build(ScAddr node);

  std::vector<ScTemplateParams> const & GetParamsSet() const
  {
    return paramsSet;
  }

  LogicExpressionNode & GetRoot()
  {
    return *root;
  }

private:
  std::vector<ScTemplateParams> paramsSet;
  std::unique_ptr<LogicExpressionNode> root;
  ScAddr conditionRoot;
  ScMemoryContext * context;
  inference::TemplateSearcher * templateSearcher;
  inference::TemplateManager  * templateManager;
  ScAddrVector argumentList;

  ScAddr atomicLogicalFormula;
  ScAddr nrelDisjunction;
  ScAddr nrelConjunction;
  ScAddr nrelNegation;
};
