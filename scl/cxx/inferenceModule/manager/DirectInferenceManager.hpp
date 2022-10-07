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

using namespace std;

namespace inference
{
// TODO: Fix issue with using interface InferenceManager
class DirectInferenceManager
{
public:
  explicit DirectInferenceManager(ScMemoryContext * ms_context);

  ScAddr applyInference(
        const ScAddr & targetStatement,
        const ScAddr & ruleSet,
        const ScAddr & argumentSet);

  ScAddr applyInference(
        const ScAddr & ruleSet,
        const ScAddr & inputStructure,
        const ScAddr & outputStructure,
        const ScAddr & targetStatement);
~DirectInferenceManager();

private:
  vector<queue<ScAddr>> createRulesQueuesListByPriority(ScAddr const & rulesSet);

  queue<ScAddr> createQueue(ScAddr const & set);

  void clearSatisfiabilityInformation(ScAddr const & rule, ScAddr const & model);
  void addSatisfiabilityInformation(ScAddr const & rule, ScAddr const & model, bool isSatisfiable);

  bool useRule(ScAddr const & rule, vector<ScAddr> /*const*/ & argumentList);

  bool isTargetAchieved(ScAddr const & target, vector<ScAddr> const & argumentList);

  bool generateStatement(ScAddr const & statement, ScTemplateParams const & templateParams);

  ScMemoryContext * ms_context;
  std::unique_ptr<TemplateManager> templateManager;
  std::unique_ptr<TemplateSearcher> templateSearcher;
  std::unique_ptr<SolutionTreeGenerator> solutionTreeGenerator;
  FormulaClassifier * formulaClassifier;
  ScAddr inputStructure;
  ScAddr outputStructure;
  ScAddr targetStatement;
};
}  // namespace inference
