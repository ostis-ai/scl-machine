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

#include "model/StatementsCheckResult.hpp"
#include "searcher/RuleConstructionsSearcher.hpp"
#include "templateSearcher/TemplateSearcher.hpp"


using namespace std;

namespace rulesApplicationModule
{

class RuleApplicationManager
{
public:
  explicit RuleApplicationManager(ScMemoryContext * context);

  void applyRules(ScAddr const & ruleSet, ScAddr const & inputStructure, ScAddr const & resultStructure);

private:
  void applyRule(ScAddr const & rule, ScAddr const & inputStructure, ScAddr const & resultStructure);

  void generateNotFoundStructures(
        StatementsCheckResult & ruleCheckResult,
        ScAddr const & resultStructure);

  void searchNotSearchedStructures(
        StatementsCheckResult & ruleCheckResult,
        ScAddr const & inputStructure,
        ScAddr const & resultStructure);

  void applyActions(
        StatementsCheckResult & ruleCheckResult,
        ScAddr const & resultStructure,
        ScAddr const & inputStructure);

  void checkOperation(
        ScAddr const & operationTuple,
        ScAddr const & inputStructure,
        ScAddr const & resultStructure,
        size_t const & nestingLevel,
        StatementsCheckResult & checkResult);

  void checkConjunction(
        ScAddr const & conjunctionNode,
        ScAddr const & inputStructure,
        ScAddr const & resultStructure,
        size_t const & nestingLevel,
        StatementsCheckResult & checkResult);

  void checkEquivalence(
        ScAddr const & equivalenceEdge,
        ScAddr const & inputStructure,
        ScAddr const & resultStructure,
        size_t const & nestingLevel,
        StatementsCheckResult & checkResult);

  void checkStatement(
        ScAddr const & statement,
        ScAddr const & inputStructure,
        ScAddr const & resultStructure,
        size_t const & nestingLevel,
        StatementsCheckResult & checkResult);

  void checkAtomicStatement(
        ScAddr const & statement,
        ScAddr const & inputStructure,
        ScAddr const & resultStructure,
        size_t const & nestingLevel,
        StatementsCheckResult & checkResult);

  void checkStatementForEmptyReplacements(
        ScAddr const & statement,
        ScAddr const & inputStructure,
        size_t const & nestingLevel,
        StatementsCheckResult & checkResult);

  void checkStatementForNonEmptyReplacements(
        ScAddr const & statement,
        vector<string> const & statementVariablesIdentifiers,
        ScAddr const & inputStructure,
        ScAddr const & resultStructure,
        size_t const & nestingLevel,
        StatementsCheckResult & checkResult);

  void applyActionsOnStatement(
        ScAddr const & statement,
        ScAddr const & inputStructure,
        ScAddr const & resultStructure,
        size_t const & nestingLevel,
        StatementsCheckResult & checkResult);

  ScAddr formActionInstance(ScAddr const & actionClass, ScAddrVector const & arguments);

  vector<ScTemplateParams> generateTemplateParams(
        vector<string> const & structureVariablesIdentifiers,
        StatementsCheckResult const & checkResult);

  ScAddr getLinkForGeneration(ScAddr const & structure, ScAddr const & varLink);

  bool allNotFoundStructuresCanBeGenerated(StatementsCheckResult const & checkResult);

  void genTemplateWithLinksContentInStructure(
        ScTemplate const & scTemplate,
        ScTemplateParams & templateParams,
        ScAddr const & templateStructure,
        ScAddr const & resultStructure,
        StatementsCheckResult & generationResult);

  ScMemoryContext * context;
  std::unique_ptr<RuleConstructionsSearcher> ruleConstructionsSearcher;
  std::unique_ptr<TemplateSearcher> templateSearcher;
  const int WAIT_TIME = 9000000;
};

} // namespace rulesApplicationModule
