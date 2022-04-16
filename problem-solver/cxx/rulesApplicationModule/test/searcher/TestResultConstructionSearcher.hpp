/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace rulesApplicationModule
{

class TestResultConstructionSearcher
{
public:
  explicit TestResultConstructionSearcher(ScMemoryContext * context);

  bool checkSeveralSimpleEquivalenceRulesResultStructure(ScAddr const & outputStructure);

  bool checkSeveralNestedEquivalenceRulesOneTemplateToGenerateResultStructure(ScAddr const & outputStructure);

  bool checkSeveralNestedEquivalenceRulesTwoTemplatesToGenerateResultStructure(ScAddr const & outputStructure);

  bool checkCannotApplyRuleTestResultConstruction(ScAddr const & outputStructure);

  bool checkRulesWithLinkInConditionResultStructure(ScAddr const & outputStructure);

  bool checkRulesWithLinkInResultResultStructure(ScAddr const & outputStructure);

  bool checkRulesWithSameResultResultStructure(ScAddr const & outputStructure);

  bool checkRuleWithLinkResultStructure(ScAddr const & outputStructure);

  bool checkActionApplicationResultStructure(ScAddr const & outputStructure, bool const & successfulCall);

private:
  ScMemoryContext * context;
};

} // namespace rulesApplicationModule
