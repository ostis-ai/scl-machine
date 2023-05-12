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
      std::shared_ptr<TemplateSearcherAbstract> templateSearcher,
      std::shared_ptr<TemplateManagerAbstract> templateManager,
      std::shared_ptr<SolutionTreeManager> solutionTreeManager,
      ScAddr const & outputStructure,
      ScAddr const & formula);

  LogicFormulaResult compute(LogicFormulaResult & result) const override;
  LogicFormulaResult find(Replacements & replacements) const;
  LogicFormulaResult generate(Replacements & replacements) override;

  ScAddr getFormula() const override
  {
    return formula;
  }

private:
  ScMemoryContext * context;

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  std::shared_ptr<TemplateManagerAbstract> templateManager;
  std::shared_ptr<SolutionTreeManager> solutionTreeManager;

  ScAddr outputStructure;
  ScAddr formula;
};
