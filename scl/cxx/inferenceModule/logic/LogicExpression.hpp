/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_template.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"

#include "manager/TemplateManager.hpp"
#include "searcher/TemplateSearcher.hpp"
#include "classifier/FormulaClassifier.hpp"

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
  bool isGenerated;
  std::map<string, std::vector<ScAddr>> replacements;
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

class ConjunctionExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit ConjunctionExpressionNode(OperandsVector & operands);
  explicit ConjunctionExpressionNode(ScMemoryContext * context, OperandsVector & operands);

  LogicExpressionResult check(ScTemplateParams & params) const override;
  LogicFormulaResult compute(LogicFormulaResult & result) const override;

  ScAddr getFormulaTemplate() const override
  {
    return ScAddr();
  }

private:
  ScMemoryContext * context;
};

class DisjunctionExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit DisjunctionExpressionNode(OperandsVector & operands);
  explicit DisjunctionExpressionNode(ScMemoryContext * context, OperandsVector & operands);

  LogicExpressionResult check(ScTemplateParams & params) const override;
  LogicFormulaResult compute(LogicFormulaResult & result) const override;

  ScAddr getFormulaTemplate() const override
  {
    return ScAddr();
  }

private:
  ScMemoryContext * context;
};

class NegationExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit NegationExpressionNode(std::unique_ptr<LogicExpressionNode> op);
  explicit NegationExpressionNode(ScMemoryContext * context, std::unique_ptr<LogicExpressionNode> op);

  LogicExpressionResult check(ScTemplateParams & params) const override;
  LogicFormulaResult compute(LogicFormulaResult & result) const override;

  ScAddr getFormulaTemplate() const override
  {
    return ScAddr();
  }

private:
  ScMemoryContext * context;
};

class ImplicationExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit ImplicationExpressionNode(OperandsVector & operands);
  explicit ImplicationExpressionNode(ScMemoryContext * context, OperandsVector & operands);

  LogicExpressionResult check(ScTemplateParams & params) const override;
  LogicFormulaResult compute(LogicFormulaResult & result) const override;

  ScAddr getFormulaTemplate() const override
  {
    return ScAddr();
  }

private:
  ScMemoryContext * context;
};

class EquivalenceExpressionNode : public OperatorLogicExpressionNode
{
public:
  explicit EquivalenceExpressionNode(OperandsVector & operands);
  explicit EquivalenceExpressionNode(ScMemoryContext * context, OperandsVector & operands);

  LogicExpressionResult check(ScTemplateParams & params) const override;
  LogicFormulaResult compute(LogicFormulaResult & result) const override;

  ScAddr getFormulaTemplate() const override
  {
    return ScAddr();
  }

private:
  ScMemoryContext * context;
};

class TemplateExpressionNode : public LogicExpressionNode
{
public:
  TemplateExpressionNode(ScMemoryContext * context, ScAddr const & formulaTemplate, TemplateSearcher * templateSearcher);
  TemplateExpressionNode(
      ScMemoryContext * context,
      ScAddr const & formulaTemplate,
      TemplateSearcher * templateSearcher,
      TemplateManager * templateManager,
      ScAddr const & outputStructure);

  LogicExpressionResult check(ScTemplateParams & params) const override;
  LogicFormulaResult compute(LogicFormulaResult & result) const override;

  LogicFormulaResult find(map<string, vector<ScAddr>> & replacements) const;
  LogicFormulaResult generate(map<string, vector<ScAddr>> & replacements) const;

  ScAddr getFormulaTemplate() const override
  {
    return formulaTemplate;
  }

private:
  ScMemoryContext * context;
  ScAddr formulaTemplate;
  inference::TemplateSearcher * templateSearcher;
  inference::TemplateManager * templateManager;
  ScAddr outputStructure;
};

class LogicExpression
{
public:
  LogicExpression(
      ScMemoryContext * context,
      TemplateSearcher * templateSearcher,
      TemplateManager * templateManager,
      ScAddrVector argumentVector);
  LogicExpression(
      ScMemoryContext * context,
      TemplateSearcher * templateSearcher,
      TemplateManager * templateManager,
      ScAddrVector argumentVector,
      ScAddr outputStructure);

  std::unique_ptr<LogicExpressionNode> build(ScAddr const & node);

  std::vector<ScTemplateParams> const & GetParamsSet() const
  {
    return paramsSet;
  }

private:
  ScMemoryContext * context;
  std::vector<ScTemplateParams> paramsSet;

  TemplateSearcher * templateSearcher;
  TemplateManager * templateManager;
  ScAddrVector argumentVector;

  ScAddr inputStructure;
  ScAddr outputStructure;
};
