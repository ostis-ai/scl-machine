/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_template.hpp>

#include "LogicExpression.hpp"

#include "searcher/templateSearcher/TemplateSearcherAbstract.hpp"
#include "manager/templateManager/TemplateManagerAbstract.hpp"
#include "manager/solutionTreeManager/SolutionTreeManagerAbstract.hpp"

using namespace inference;

class TemplateExpressionNode : public LogicExpressionNode
{
public:
  TemplateExpressionNode(
      ScMemoryContext * context,
      std::shared_ptr<TemplateSearcherAbstract> templateSearcher,
      std::shared_ptr<TemplateManagerAbstract> templateManager,
      std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager,
      ScAddr const & outputStructure,
      ScAddr const & formula);

  void compute(LogicFormulaResult & result) const override;
  // TODO: remove useless method. Use compute instead of find
  LogicFormulaResult find(Replacements & replacements) const;
  LogicFormulaResult generate(Replacements & replacements) override;

  ScAddr getFormula() const override
  {
    return formula;
  }

private:
  ScMemoryContext * context;

  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  std::unique_ptr<TemplateSearcherAbstract> templateSearcherGeneral;
  std::shared_ptr<TemplateManagerAbstract> templateManager;
  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;

  ScAddr outputStructure;
  ScAddr formula;
  void generateByReplacements(
      Replacements const & replacements,
      LogicFormulaResult & result,
      size_t & count,
      ScAddrHashSet const & formulaVariables,
      Replacements & searchResult,
      Replacements & generatedReplacements);

  void generateByParams(
      ScTemplateParams const & params,
      ScAddrHashSet const & formulaVariables,
      Replacements & generatedReplacements,
      LogicFormulaResult & result,
      size_t & count);
  void processTemplateParams(
      vector<ScTemplateParams> const & paramsVector,
      ScAddrHashSet const & formulaVariables,
      LogicFormulaResult & result,
      size_t & count,
      Replacements & searchResult,
      Replacements & generatedReplacements);
  Replacements getSearchResultWithoutReplacementsIfNeeded() const;
  void fillOutputStructure(
      ScAddrHashSet const & formulaVariables,
      Replacements const & replacements,
      Replacements const & resultWithoutReplacements,
      Replacements const & searchResult);
  void addFormulaConstantsToOutputStructure();
  void addToOutputStructure(Replacements const & replacements, ScAddrHashSet const & variables);
  void addToOutputStructure(ScAddrHashSet const & elements);
  void addToOutputStructure(ScTemplateResultItem const & item);
  void addToOutputStructure(ScAddr const & element);
};
