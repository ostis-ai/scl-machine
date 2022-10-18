/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_test.hpp"
#include "scs_loader.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "manager/DirectInferenceManager.hpp"
#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

namespace directInferenceManagerTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/ManagerModule/";
const std::string TEMPLATE_REPLY_TARGET = "template_reply_target";
const std::string RULE_CLASS = "concept_answer_on_standard_message_rule_class_by_priority";
const std::string MESSAGE_SINGLETON = "message_singleton";

using InferenceManagerTest = ScMemoryTest;

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

TEST_F(InferenceManagerTest, SuccessApplyInference)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "lr_concept_weather_request_message_1.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                      + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);
  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));
}

TEST_F(InferenceManagerTest, RuleNotFound)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "incorrect_rule_class_logic_rule.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                             + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

TEST_F(InferenceManagerTest, RuleNotUsed)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleNotUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "lr_concept_weather_request_message_1.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                        + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

TEST_F(InferenceManagerTest, IncorrectElseStatementStructure)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "incorrect_else_statement_logic_rule.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                      + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

TEST_F(InferenceManagerTest, IncorrectIfStatementStructure)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerRuleUsedTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "incorrect_if_statement_logic_rule.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                             + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

TEST_F(InferenceManagerTest, IncorrectTargetTemplate)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerIncorrectTemplateTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "lr_concept_weather_request_message_1.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                      + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

TEST_F(InferenceManagerTest, IncorrectArgumentSet)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceManagerIncorrectArgumentSetTest.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "lr_concept_weather_request_message_1.scs");
  loader.loadScsFile(context,TEST_FILES_DIR_PATH
                                      + "concept_answer_on_standard_message_rule_class_by_priority.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TEMPLATE_REPLY_TARGET);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULE_CLASS);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(MESSAGE_SINGLETON);
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}
} //namespace directInferenceAgentTest
