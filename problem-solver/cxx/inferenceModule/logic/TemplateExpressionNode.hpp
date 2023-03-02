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
#include "manager/SolutionTreeManager.hpp"

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
      SolutionTreeManager * solutionTreeManager,
      ScAddr const & outputStructure,
      ScAddr const & rule);

  LogicExpressionResult check(ScTemplateParams & params) const override;
  LogicFormulaResult compute(LogicFormulaResult & result) const override;
  LogicFormulaResult find(Replacements & replacements) const;
  LogicFormulaResult generate(Replacements & replacements) const override;

  ScAddr getFormulaTemplate() const override
  {
    return formulaTemplate;
  }

private:
  ScMemoryContext * context;
  ScAddr formulaTemplate;
  TemplateSearcher * templateSearcher;
  TemplateManager * templateManager;
  SolutionTreeManager * solutionTreeManager;
  ScAddr outputStructure;
  ScAddr rule;
};
