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
std::string const TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/ManagerModule/";

std::string const TARGET_TEMPLATE = "target_template";
std::string const RULES_SET = "rules_set";
std::string const ARGUMENT_SET = "argument_set";

using InferenceManagerTest = ScMemoryTest;

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

TEST_F(InferenceManagerTest, SuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSimpleRuleTest.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr outputStructure = context.CreateNode(ScType::NodeConstStruct);
  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet, outputStructure);
  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));
}

TEST_F(InferenceManagerTest, SuccessGenerateInferenceConclusion)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "conclusionGenerationTest.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr outputStructure = context.CreateNode(ScType::NodeConstStruct);
  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet, outputStructure);
  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));
  ScTemplate conclusionTemplate;
  ScTemplateSearchResult conclusionSearchResult;
  context.HelperBuildTemplate(conclusionTemplate,targetTemplate);
  EXPECT_TRUE(context.HelperSearchTemplate(conclusionTemplate, conclusionSearchResult));
}

TEST_F(InferenceManagerTest, RuleNotUsed)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "ruleNotUsedTest.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr outputStructure = context.CreateNode(ScType::NodeConstStruct);
  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet, outputStructure);
  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

TEST_F(InferenceManagerTest, TargetNotAchieved)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "targetNotAchievedTest.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr outputStructure = context.CreateNode(ScType::NodeConstStruct);
  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet, outputStructure);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

// Replace not working
TEST_F(InferenceManagerTest, ReplaceNotWorking)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "replaceNotWork.scs");
  initialize();

  ScAddr targetTemplate = context.HelperResolveSystemIdtf(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.HelperResolveSystemIdtf(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr outputStructure = context.CreateNode(ScType::NodeConstStruct);
  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet, outputStructure);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_solution, answer, ScType::EdgeAccessConstNegPerm));
  EXPECT_FALSE(
      context.HelperCheckEdge(InferenceKeynodes::concept_solution, answer, ScType::EdgeAccessConstPosPerm));
}

}  // namespace directInferenceManagerTest
