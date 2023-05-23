/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_test.hpp"
#include "scs_loader.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "manager/inferenceManager/DirectInferenceManagerTarget.hpp"
#include "keynodes/InferenceKeynodes.hpp"
#include "factory/InferenceManagerFactory.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

using namespace inference;

namespace directInferenceManagerTest
{
ScsLoader loader;
std::string const TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/ManagerModule/";

std::string const TARGET_TEMPLATE = "target_template";
std::string const RULES_SET = "rules_set";
std::string const ARGUMENT_SET = "argument_set";
std::string const INPUT_STRUCTURE = "input_structure";

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

  ScAddr inputStructure = context.HelperResolveSystemIdtf(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {inputStructure}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());
  ScAddr targetClass = context.HelperFindBySystemIdtf("target_node_class");
  EXPECT_TRUE(targetClass.IsValid());

  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));
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

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_ALL_KB};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));
  ScTemplate conclusionTemplate;
  ScTemplateSearchResult conclusionSearchResult;
  context.HelperBuildTemplate(conclusionTemplate, targetTemplate);
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

  ScAddr inputStructure = context.HelperResolveSystemIdtf(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {inputStructure}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

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

  ScAddr inputStructure = context.HelperResolveSystemIdtf(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {inputStructure}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

TEST_F(InferenceManagerTest, ReplacementsTest)
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

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_ALL_KB};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstNegPerm));
}

// Add dialog to arguments_set and test will pass
TEST_F(InferenceManagerTest, DISABLED_ConclusionArgumentsTest)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "ConclusionArgumentsTest.scs");

  initialize();

  ScAddr targetStructure = context.HelperResolveSystemIdtf("inference_target");
  EXPECT_TRUE(targetStructure.IsValid());

  ScAddr formulasSet = context.HelperResolveSystemIdtf("rules_sets");
  EXPECT_TRUE(formulasSet.IsValid());

  ScAddr argumentSet = context.HelperResolveSystemIdtf(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr inputStructure = context.HelperResolveSystemIdtf(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{
      formulasSet, argumentVector, {inputStructure}, outputStructure, targetStructure};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->createSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));

  ScAddr dialog = context.HelperFindBySystemIdtf("dialog");
  ScAddr dialogClass = context.HelperFindBySystemIdtf("concept_dialog_class");
  EXPECT_TRUE(dialog.IsValid());
  EXPECT_TRUE(dialogClass.IsValid());

  EXPECT_TRUE(context.HelperCheckEdge(dialogClass, dialog, ScType::EdgeAccessConstPosPerm));
}

TEST_F(InferenceManagerTest, SolutionOutputStrcuture)
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

  ScAddr inputStructure = context.HelperResolveSystemIdtf(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & output = context.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {inputStructure}, output, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr solution = inferenceManager->getSolutionTreeManager()->createSolution(output, targetAchieved);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  // Check if nrel_output_structure exists
  ScIterator5Ptr solutionOutputIterator = context.Iterator5(
      solution,
      ScType::EdgeDCommonConst,
      ScType::NodeConstStruct,
      ScType::EdgeAccessConstPosPerm,
      InferenceKeynodes::nrel_output_structure);

  EXPECT_TRUE(solutionOutputIterator->Next());
  ScAddr outputStructure = solutionOutputIterator->Get(2);
  EXPECT_TRUE(outputStructure.IsValid());

  // Check class in output structure, expect target_node_class
  ScIterator3Ptr outputStructureClassIterator =
      context.Iterator3(outputStructure, ScType::EdgeAccessConstPosPerm, ScType::NodeConstClass);
  EXPECT_TRUE(outputStructureClassIterator->Next());

  ScAddr outputStructureClass = outputStructureClassIterator->Get(2);
  EXPECT_TRUE(outputStructureClass.IsValid());

  ScAddr targetClass = context.HelperFindBySystemIdtf("target_node_class");
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(targetClass == outputStructureClass);

  // Check target_node_class element in output structure, expect argument
  ScIterator5Ptr outputStructureClassElementIterator = context.Iterator5(
      outputStructureClass,
      ScType::EdgeAccessConstPosPerm,
      ScType::NodeConst,
      ScType::EdgeAccessConstPosPerm,
      outputStructure);
  EXPECT_TRUE(outputStructureClassElementIterator->Next());

  ScAddr outputStructureClassElement = outputStructureClassElementIterator->Get(2);
  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());
  EXPECT_TRUE(outputStructureClassElement.IsValid());
  EXPECT_TRUE(argument == outputStructureClassElement);

  // Check if edge between target_node_class and argument exists in output structure
  ScIterator5Ptr edgeOutputStructureIterator = context.Iterator5(
      outputStructureClass,
      ScType::EdgeAccessConstPosPerm,
      outputStructureClassElement,
      ScType::EdgeAccessConstPosPerm,
      outputStructure);
  EXPECT_TRUE(edgeOutputStructureIterator->Next());
  EXPECT_FALSE(edgeOutputStructureIterator->Next());

  EXPECT_FALSE(solutionOutputIterator->Next());
}

}  // namespace directInferenceManagerTest
