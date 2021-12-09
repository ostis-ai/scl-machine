/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-test-framework/sc_test_unit.hpp"
#include "catch2/catch.hpp"
#include "builder/src/scs_loader.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "manager/DirectInferenceManager.hpp"
#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

namespace directInferenceManagerTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/ManagerModule/";
const std::string SC_MEMORY_INI = "sc-memory.ini";
const std::string TEMPLATE_REPLY_TARGET = "template_reply_target";
const std::string RULE_CLASS = "concept_answer_on_standard_message_rule_class_by_priority";
const std::string MESSAGE_SINGLETON = "message_singleton";

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

TEST_CASE("success apply inference", "[inference manager test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "lr_concept_weather_request_message_1.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                      + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  REQUIRE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  REQUIRE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);
  REQUIRE(answer.IsValid());
  REQUIRE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("rule not found", "[inference manager test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "incorrect_rule_class_logic_rule.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                             + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  REQUIRE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  REQUIRE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  REQUIRE(answer.IsValid());
  REQUIRE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("rule not used", "[inference manager test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleNotUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "lr_concept_weather_request_message_1.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                        + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  REQUIRE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  REQUIRE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  REQUIRE(answer.IsValid());
  REQUIRE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("incorrect else statement structure", "[inference manager test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "incorrect_else_statement_logic_rule.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                      + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  REQUIRE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  REQUIRE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  REQUIRE(answer.IsValid());
  REQUIRE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("incorrect if statement structure", "[inference manager test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "incorrect_if_statement_logic_rule.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                             + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  REQUIRE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  REQUIRE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  REQUIRE(answer.IsValid());
  REQUIRE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("incorrect target template", "[inference manager test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerIncorrectTemplateTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "lr_concept_weather_request_message_1.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                      + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  REQUIRE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  REQUIRE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  REQUIRE(answer.IsValid());
  REQUIRE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("incorrect argument set", "[inference manager test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerIncorrectArgumentSetTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "lr_concept_weather_request_message_1.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                      + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  REQUIRE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  REQUIRE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  REQUIRE(answer.IsValid());
  REQUIRE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
} //namespace directInferenceAgentTest
