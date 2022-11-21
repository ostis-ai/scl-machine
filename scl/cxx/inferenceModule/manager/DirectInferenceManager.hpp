/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <queue>
#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

#include "generator/SolutionTreeGenerator.hpp"
#include "manager/TemplateManager.hpp"
#include "searcher/TemplateSearcher.hpp"
#include "classifier/FormulaClassifier.hpp"

namespace inference
{
// TODO: Fix issue with using interface InferenceManager
class DirectInferenceManager
{
public:
  explicit DirectInferenceManager(ScMemoryContext * ms_context);

  ScAddr applyInference(
      ScAddr const & targetStructure,
      ScAddr const & formulasSet,
      ScAddr const & inputStructure,
      ScAddr const & outputStructure);

private:
  vector<queue<ScAddr>> createFormulasQueuesListByPriority(ScAddr const & formulasSet);

  queue<ScAddr> createQueue(ScAddr const & set);

  void clearSatisfiabilityInformation(ScAddr const & formula, ScAddr const & model);
  void addSatisfiabilityInformation(ScAddr const & rule, ScAddr const & model, bool isSatisfiable);

  bool useFormula(ScAddr const & rule, ScAddrVector /*const*/ & argumentVector);

  bool isTargetAchieved(ScAddr const & targetStructure, ScAddrVector const & argumentVector);

  bool generateStatement(ScAddr const & statement, ScTemplateParams const & templateParams);

  ScMemoryContext * ms_context;
  std::unique_ptr<TemplateManager> templateManager;
  std::unique_ptr<TemplateSearcher> templateSearcher;
  std::unique_ptr<SolutionTreeGenerator> solutionTreeGenerator;
  FormulaClassifier * formulaClassifier;
  ScAddr inputStructure;
  ScAddr outputStructure;
  ScAddr targetStructure;
};
}  // namespace inference
