/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_template.hpp>

#include "LogicExpression.hpp"

#include "searcher/TemplateSearcher.hpp"
#include "manager/TemplateManager.hpp"

using namespace inference;

class TemplateExpressionNode : public LogicExpressionNode
{
public:
  TemplateExpressionNode(
      ScMemoryContext * context,
      ScAddr const & formulaTemplate,
      TemplateSearcher * templateSearcher);
  TemplateExpressionNode(
      ScMemoryContext * context,
      ScAddr const & formulaTemplate,
      TemplateSearcher * templateSearcher,
      TemplateManager * templateManager,
      ScAddr const & outputStructure);

  LogicExpressionResult check(ScTemplateParams & params) const override;
  LogicFormulaResult compute(LogicFormulaResult & result) const override;

  LogicFormulaResult find(std::map<std::string, ScAddrVector> & replacements) const;
  LogicFormulaResult generate(std::map<std::string, ScAddrVector> & replacements) const;

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
