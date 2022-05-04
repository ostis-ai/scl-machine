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
#include "classifier/FormulaClassifier.hpp"

#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>

using namespace inference;

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
    std::map<string, std::vector<ScAddr>> replacements;
};

class LogicExpressionNode
{
public:
  virtual LogicExpressionResult check(ScTemplateParams params) const = 0;
  virtual LogicFormulaResult compute(ScTemplateParams params) const = 0;
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
  explicit AndExpressionNode(ScMemoryContext * context, OperandsVectorType & operands);

  LogicExpressionResult check(ScTemplateParams params) const override;
  LogicFormulaResult compute(ScTemplateParams params) const override;
private:
  ScMemoryContext * context;
};

class OrExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit OrExpressionNode(OperandsVectorType & operands);
  explicit OrExpressionNode(ScMemoryContext * context, OperandsVectorType & operands);

  LogicExpressionResult check(ScTemplateParams params) const override;
  LogicFormulaResult compute(ScTemplateParams params) const override;
private:
  ScMemoryContext * context;
};

class NotExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit NotExpressionNode(std::unique_ptr<LogicExpressionNode> op);
  explicit NotExpressionNode(ScMemoryContext * context, std::unique_ptr<LogicExpressionNode> op);

  LogicExpressionResult check(ScTemplateParams params) const override;
  LogicFormulaResult compute(ScTemplateParams params) const override;
private:
  ScMemoryContext * context;
};

class ImplicationExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit ImplicationExpressionNode(OperandsVectorType & operands);
  explicit ImplicationExpressionNode(ScMemoryContext * context, OperandsVectorType & operands);

  LogicExpressionResult check(ScTemplateParams params) const override;
  LogicFormulaResult compute(ScTemplateParams params) const override;
private:
  ScMemoryContext * context;
};

class EquivalenceExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit EquivalenceExpressionNode(OperandsVectorType & operands);
  explicit EquivalenceExpressionNode(ScMemoryContext * context, OperandsVectorType & operands);

  LogicExpressionResult check(ScTemplateParams params) const override;
  LogicFormulaResult compute(ScTemplateParams params) const override;
private:
  ScMemoryContext * context;
};

class TemplateExpressionNode : public LogicExpressionNode
{
public:
  TemplateExpressionNode(ScMemoryContext * context, ScAddr formulaTemplate, TemplateSearcher * templateSearcher);

  LogicExpressionResult check(ScTemplateParams params) const override;
  LogicFormulaResult compute(ScTemplateParams params) const override;

  LogicFormulaResult find(map<string, vector<ScAddr>> replacements) const;

  ScAddr getFormulaTemplate(){ return formulaTemplate;}

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
  LogicExpression(
        ScMemoryContext * context,
        TemplateSearcher * templateSearcher,
        TemplateManager * templateManager,
        ScAddrVector argumentList,
        ScAddr outputStructure
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

  ScAddr inputStructure;
  ScAddr outputStructure;
};
