/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "searcher/TemplateSearcherAbstract.hpp"
#include "manager/SolutionTreeManager.hpp"
#include "manager/TemplateManager.hpp"

#include "logic/LogicExpressionNode.hpp"

namespace inference
{
using ScAddrQueue = std::queue<ScAddr>;

/// Class to control formulas iteration strategy: when to top applying rules and how to iterate over them
class FormulasIterationStrategyAbstract
{
public:
  explicit FormulasIterationStrategyAbstract(ScMemoryContext * context);

  virtual ~FormulasIterationStrategyAbstract() = default;

  void setTemplateSearcher(std::shared_ptr<TemplateSearcherAbstract> searcher);
  void setTemplateManager(std::shared_ptr<TemplateManagerAbstract> manager);
  void setSolutionTreeManager(std::shared_ptr<SolutionTreeManager> searcher);

  void setArguments(ScAddr const & otherArguments);

  std::shared_ptr<SolutionTreeManager> getSolutionTreeManager();

  virtual bool applyIterationStrategy(ScAddr const & formulasSet, ScAddr const & outputStructure) = 0;

  LogicFormulaResult useFormula(
        ScAddr const & formula,
        ScAddrVector & argumentVector,
        ScAddr const & outputStructure);

  void fillFormulaFixedArgumentsIdentifiers(ScAddr const & formula) const;

  ScAddrVector formArgumentsVector() const;

  vector<ScAddrQueue> createFormulasQueuesListByPriority(ScAddr const & formulasSet);

  ScAddrQueue createQueue(ScAddr const & set);

protected:
  ScMemoryContext * context;

  std::shared_ptr<TemplateManagerAbstract> templateManager;
  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  std::shared_ptr<SolutionTreeManager> solutionTreeManager;

  ScAddr arguments;
  std::unordered_set<ScAddr, ScAddrHashFunc<::size_t>> outputStructureElements;
};
}
