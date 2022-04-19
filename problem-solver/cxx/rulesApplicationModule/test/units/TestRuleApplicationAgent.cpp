/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <sc_test.hpp>
#include "builder/src/scs_loader.hpp"
#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "agent/RulesApplicationAgent.hpp"
#include "keynodes/Keynodes.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "test/searcher/TestResultConstructionSearcher.hpp"
#include "test/keynodes/TestKeynodes.hpp"
#include "test/agents/ConnectionCheckAgent.hpp"

using namespace rulesApplicationModule;

ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = RULES_APPLICATION_MODULE_TEST_SRC_PATH "/testStructures/rulesApplicationAgent/";
const std::string SC_MEMORY_INI = "sc-memory.ini";
const int WAIT_TIME = 7000;

using RulesApplicationAgentTest = ScMemoryTest;

void initialize()
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  rulesApplicationModule::Keynodes::InitGlobal();
  rulesApplicationModule::TestKeynodes::InitGlobal();

  ScAgentInit(true);
  SC_AGENT_REGISTER(rulesApplicationModule::RulesApplicationAgent);
}

void shutdown()
{
  SC_AGENT_UNREGISTER(rulesApplicationModule::RulesApplicationAgent);
}

TEST_F(RulesApplicationAgentTest, check_dynamic_arguments)
{
  ScMemoryContext context(sc_access_lvl_make_min, "check_dynamic_arguments");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testErrorInRuleStructure.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, atomic_statement_is_not_template)
{
  ScMemoryContext context(sc_access_lvl_make_min, "atomic_statement_is_not_template");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testAtomicStatementIsNotTemplate.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, test_several_simple_equivalence_rules)
{
  ScMemoryContext context(sc_access_lvl_make_min, "test_several_simple_equivalence_rules");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testSeveralSimpleEquivalenceRules.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkSeveralSimpleEquivalenceRulesResultStructure(testOutputStructure));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, test_several_nested_equivalence_rules_one_template_to_generate)
{
  ScMemoryContext context(sc_access_lvl_make_min, "test_several_nested_equivalence_rules_one_template_to_generate");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "test_several_nested_equivalence_rules_one_template_to_generate.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkSeveralNestedEquivalenceRulesOneTemplateToGenerateResultStructure(testOutputStructure));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, test_several_nested_equivalence_rules_two_template_to_generate)
{
  ScMemoryContext context(sc_access_lvl_make_min, "test_several_nested_equivalence_rules_two_template_to_generate");

  loader.loadScsFile(
        context,
        TEST_FILES_DIR_PATH + "testSeveralNestedEquivalenceRulesTwoTemplatesToGenerate.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkSeveralNestedEquivalenceRulesTwoTemplatesToGenerateResultStructure(testOutputStructure));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, not_apply_rule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "not_apply_rule");

  loader.loadScsFile(
        context,
        TEST_FILES_DIR_PATH + "testCannotApplyRule.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkCannotApplyRuleTestResultConstruction(testOutputStructure));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, rules_with_link_in_condition)
{
  ScMemoryContext context(sc_access_lvl_make_min, "rules_with_link_in_condition");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testRulesWithLinkInCondition.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkRulesWithLinkInConditionResultStructure(testOutputStructure));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, rules_with_link_in_result)
{
  ScMemoryContext context(sc_access_lvl_make_min, "rules_with_link_in_result");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testRulesWithLinkInResult.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkRulesWithLinkInResultResultStructure(testOutputStructure));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, rules_with_same_result)
{
  ScMemoryContext context(sc_access_lvl_make_min, "rules_with_same_result");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testRulesWithSameResult.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkRulesWithSameResultResultStructure(testOutputStructure));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, rule_with_link)
{
  ScMemoryContext context(sc_access_lvl_make_min, "rule_with_link");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testRuleWithLink.scs");
  initialize();

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkRuleWithLinkResultStructure(testOutputStructure));

  shutdown();
  context.Destroy();
}

TEST_F(RulesApplicationAgentTest, call_successful)
{
  ScMemoryContext context(sc_access_lvl_make_min, "call_successful");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testActionSuccessfulCall.scs");
  initialize();
  SC_AGENT_REGISTER(ConnectionCheckAgent)

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkActionApplicationResultStructure(testOutputStructure, true));

  SC_AGENT_UNREGISTER(ConnectionCheckAgent)
  shutdown();
}

TEST_F(RulesApplicationAgentTest, call_unsuccessful)
{
  ScMemoryContext context(sc_access_lvl_make_min, "call_unsuccessful");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testActionUnsuccessfulCall.scs");
  initialize();
  SC_AGENT_REGISTER(ConnectionCheckAgent)

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action");

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_question_node);

  EXPECT_TRUE(utils::AgentUtils::waitAgentResult(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test_question_node,
        ScType::EdgeAccessConstPosPerm));

  ScAddr testOutputStructure = context.HelperFindBySystemIdtf("test_output_structure");
  TestResultConstructionSearcher resultSearcher = TestResultConstructionSearcher(&context);
  EXPECT_TRUE(resultSearcher.checkActionApplicationResultStructure(testOutputStructure, false));

  SC_AGENT_UNREGISTER(ConnectionCheckAgent)
  shutdown();
  context.Destroy();
}
