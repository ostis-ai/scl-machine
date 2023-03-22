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

// Здесь будет управление обходом формул (шаблоном цели)
namespace inference
{
using ScAddrQueue = std::queue<ScAddr>;

class FormulasIterationStrategyAbstract
{
public:
  explicit FormulasIterationStrategyAbstract(ScMemoryContext * context);

  virtual ~FormulasIterationStrategyAbstract() = default;

  void setTemplateSearcher(std::unique_ptr<TemplateSearcherAbstract> searcher);
  void setTemplateManager(std::unique_ptr<TemplateManager> searcher);
  void setSolutionTreeManager(std::unique_ptr<SolutionTreeManager> searcher);

  void setArguments(ScAddr const & otherArguments);

  std::unique_ptr<SolutionTreeManager> getSolutionTreeManager();

  virtual bool applyIterationStrategy(ScAddr const & formulasSet, ScAddr const & outputStructure) = 0;

  LogicFormulaResult useFormula(
        ScAddr const & rule,
        ScAddrVector & argumentVector,
        ScAddr const & outputStructure);

  vector<ScAddrQueue> createFormulasQueuesListByPriority(ScAddr const & formulasSet);

  ScAddrQueue createQueue(ScAddr const & set);

protected:
  ScMemoryContext * context;

  std::unique_ptr<TemplateManager> templateManager;
  std::unique_ptr<TemplateSearcherAbstract> templateSearcher;
  std::unique_ptr<SolutionTreeManager> solutionTreeManager;

  ScAddr arguments;
};
}
