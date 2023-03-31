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
      std::shared_ptr<TemplateSearcherAbstract> templateSearcher);
  TemplateExpressionNode(
      ScMemoryContext * context,
      ScAddr const & formulaTemplate,
      std::shared_ptr<TemplateSearcherAbstract> templateSearcher,
      std::shared_ptr<TemplateManager> templateManager,
      std::shared_ptr<SolutionTreeManager> solutionTreeManager,
      ScAddr const & outputStructure,
      ScAddr const & formula);

  // TODO(MksmOrlov): check method is never used
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
  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  std::shared_ptr<TemplateManager> templateManager;
  std::shared_ptr<SolutionTreeManager> solutionTreeManager;
  ScAddr outputStructure;
  ScAddr formula;
};
