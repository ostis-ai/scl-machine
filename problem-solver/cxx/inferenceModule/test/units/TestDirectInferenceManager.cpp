/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ConfigGenerators.hpp"

#include "factory/InferenceManagerFactory.hpp"

#include "keynodes/InferenceKeynodes.hpp"

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>

using namespace inference;

namespace directInferenceManagerTest
{
ScsLoader loader;
std::string const TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/ManagerModule/";

std::string const TARGET_TEMPLATE = "target_template";
std::string const RULES_SET = "rules_set";
std::string const ARGUMENT_SET = "argument_set";
std::string const INPUT_STRUCTURE = "input_structure";

class InferenceManagerTest
  : public ScMemoryTest
  , public testing::WithParamInterface<std::shared_ptr<generatorTest::ConfigGenerator>>
{
};

std::shared_ptr<generatorTest::ConfigGenerator> generators[] = {
    std::make_shared<generatorTest::ConfigGenerator>(),
    std::make_shared<generatorTest::ConfigGeneratorSearchWithReplacements>(),
    std::make_shared<generatorTest::ConfigGeneratorSearchWithoutReplacements>()};

INSTANTIATE_TEST_SUITE_P(
    InferenceManagerTestInitiator,
    InferenceManagerTest,
    testing::ValuesIn(generators),
    [](testing::TestParamInfo<std::shared_ptr<generatorTest::ConfigGenerator>> const & testParamInfo) {
      return testParamInfo.param->getName();
    });

TEST_P(InferenceManagerTest, SuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSimpleRuleTest.scs");

  ScAddr targetTemplate = context.ResolveElementSystemIdentifier(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.ResolveElementSystemIdentifier(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.ResolveElementSystemIdentifier(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr inputStructure = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {inputStructure}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->generateSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, answer, ScType::ConstPermPosArc));

  ScAddr argument = context.SearchElementBySystemIdentifier("argument");
  EXPECT_TRUE(argument.IsValid());
  ScAddr targetClass = context.SearchElementBySystemIdentifier("target_node_class");
  EXPECT_TRUE(targetClass.IsValid());

  EXPECT_TRUE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));
}

TEST_P(InferenceManagerTest, SuccessGenerateInferenceConclusion)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "conclusionGenerationTest.scs");

  ScAddr targetTemplate = context.ResolveElementSystemIdentifier(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.ResolveElementSystemIdentifier(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.ResolveElementSystemIdentifier(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_ALL_KB});
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->generateSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, answer, ScType::ConstPermPosArc));
  ScTemplate conclusionTemplate;
  ScTemplateSearchResult conclusionSearchResult;
  context.BuildTemplate(conclusionTemplate, targetTemplate);
  EXPECT_TRUE(context.SearchByTemplate(conclusionTemplate, conclusionSearchResult));
}

TEST_P(InferenceManagerTest, RuleNotUsed)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "ruleNotUsedTest.scs");

  ScAddr targetTemplate = context.ResolveElementSystemIdentifier(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.ResolveElementSystemIdentifier(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.ResolveElementSystemIdentifier(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr inputStructure = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {inputStructure}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->generateSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, answer, ScType::ConstPermNegArc));
}

TEST_P(InferenceManagerTest, TargetNotAchieved)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "targetNotAchievedTest.scs");

  ScAddr targetTemplate = context.ResolveElementSystemIdentifier(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.ResolveElementSystemIdentifier(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.ResolveElementSystemIdentifier(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr inputStructure = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {inputStructure}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->generateSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, answer, ScType::ConstPermNegArc));
}

TEST_P(InferenceManagerTest, ReplacementsTest)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "replaceNotWork.scs");

  ScAddr targetTemplate = context.ResolveElementSystemIdentifier(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.ResolveElementSystemIdentifier(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.ResolveElementSystemIdentifier(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_ALL_KB});
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {}, outputStructure, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->generateSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, answer, ScType::ConstPermNegArc));
}

// Add dialog to arguments_set and test will pass
TEST_P(InferenceManagerTest, DISABLED_ConclusionArgumentsTest)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "ConclusionArgumentsTest.scs");

  ScAddr targetStructure = context.ResolveElementSystemIdentifier("inference_target");
  EXPECT_TRUE(targetStructure.IsValid());

  ScAddr formulasSet = context.ResolveElementSystemIdentifier("rules_sets");
  EXPECT_TRUE(formulasSet.IsValid());

  ScAddr argumentSet = context.ResolveElementSystemIdentifier(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr inputStructure = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  InferenceParams const & inferenceParams{
      formulasSet, argumentVector, {inputStructure}, outputStructure, targetStructure};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr answer = inferenceManager->getSolutionTreeManager()->generateSolution(outputStructure, targetAchieved);

  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, answer, ScType::ConstPermPosArc));

  ScAddr dialog = context.SearchElementBySystemIdentifier("dialog");
  ScAddr dialogClass = context.SearchElementBySystemIdentifier("concept_dialog_class");
  EXPECT_TRUE(dialog.IsValid());
  EXPECT_TRUE(dialogClass.IsValid());

  EXPECT_TRUE(context.CheckConnector(dialogClass, dialog, ScType::ConstPermPosArc));
}

TEST_P(InferenceManagerTest, SolutionOutputStrcuture)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSimpleRuleTest.scs");

  ScAddr targetTemplate = context.ResolveElementSystemIdentifier(TARGET_TEMPLATE);
  EXPECT_TRUE(targetTemplate.IsValid());

  ScAddr ruleSet = context.ResolveElementSystemIdentifier(RULES_SET);
  EXPECT_TRUE(ruleSet.IsValid());

  ScAddr argumentSet = context.ResolveElementSystemIdentifier(ARGUMENT_SET);
  EXPECT_TRUE(argumentSet.IsValid());

  ScAddr inputStructure = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE);
  EXPECT_TRUE(inputStructure.IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  ScAddrVector const & argumentVector = utils::IteratorUtils::getAllWithType(&context, argumentSet, ScType::Node);
  ScAddr const & output = context.GenerateNode(ScType::ConstNodeStructure);
  InferenceParams const & inferenceParams{ruleSet, argumentVector, {inputStructure}, output, targetTemplate};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager =
      InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);
  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  ScAddr solution = inferenceManager->getSolutionTreeManager()->generateSolution(output, targetAchieved);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  // Check if nrel_output_structure exists
  ScIterator5Ptr solutionOutputIterator = context.CreateIterator5(
      solution,
      ScType::ConstCommonArc,
      ScType::ConstNodeStructure,
      ScType::ConstPermPosArc,
      InferenceKeynodes::nrel_output_structure);

  EXPECT_TRUE(solutionOutputIterator->Next());
  ScAddr outputStructure = solutionOutputIterator->Get(2);
  EXPECT_TRUE(outputStructure.IsValid());

  // Check class in output structure, expect target_node_class
  ScIterator3Ptr outputStructureClassIterator =
      context.CreateIterator3(outputStructure, ScType::ConstPermPosArc, ScType::ConstNodeClass);
  EXPECT_TRUE(outputStructureClassIterator->Next());

  ScAddr outputStructureClass = outputStructureClassIterator->Get(2);
  EXPECT_TRUE(outputStructureClass.IsValid());

  ScAddr targetClass = context.SearchElementBySystemIdentifier("target_node_class");
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(targetClass == outputStructureClass);

  // Check target_node_class element in output structure, expect argument
  ScIterator5Ptr outputStructureClassElementIterator = context.CreateIterator5(
      outputStructureClass,
      ScType::ConstPermPosArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      outputStructure);
  EXPECT_TRUE(outputStructureClassElementIterator->Next());

  ScAddr outputStructureClassElement = outputStructureClassElementIterator->Get(2);
  ScAddr argument = context.SearchElementBySystemIdentifier("argument");
  EXPECT_TRUE(argument.IsValid());
  EXPECT_TRUE(outputStructureClassElement.IsValid());
  EXPECT_TRUE(argument == outputStructureClassElement);

  // Check if arc between target_node_class and argument exists in output structure
  ScIterator5Ptr arcOutputStructureIterator = context.CreateIterator5(
      outputStructureClass,
      ScType::ConstPermPosArc,
      outputStructureClassElement,
      ScType::ConstPermPosArc,
      outputStructure);
  EXPECT_TRUE(arcOutputStructureIterator->Next());
  EXPECT_FALSE(arcOutputStructureIterator->Next());

  EXPECT_FALSE(solutionOutputIterator->Next());
}

TEST_P(InferenceManagerTest, conclusionContainsArcReplacementFromPremise)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "conclusionContainsArcReplacementFromPremise.scs");

  ScAddr const & inputStructure = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE);
  ScAddr const & targetTemplate = context.ResolveElementSystemIdentifier(TARGET_TEMPLATE);
  ScAddrUnorderedSet inputStructures{inputStructure};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(RULES_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & set1 = context.SearchElementBySystemIdentifier("set1");
  ScAddr const & set2 = context.SearchElementBySystemIdentifier("set2");
  EXPECT_TRUE(set1.IsValid());
  EXPECT_TRUE(set2.IsValid());
  ScAddr const & nrelInclusion = context.SearchElementBySystemIdentifier("nrel_inclusion");
  ScAddr const & nrelSubset = context.SearchElementBySystemIdentifier("nrel_subset");
  EXPECT_TRUE(nrelInclusion.IsValid());
  EXPECT_TRUE(nrelSubset.IsValid());
  {
    auto const & sets12IteratorBefore = context.CreateIterator3(set1, ScType::ConstCommonArc, set2);
    EXPECT_TRUE(sets12IteratorBefore->Next());
    EXPECT_TRUE(context.CheckConnector(nrelInclusion, sets12IteratorBefore->Get(1), ScType::ConstPermPosArc));
    EXPECT_FALSE(context.CheckConnector(nrelSubset, sets12IteratorBefore->Get(1), ScType::ConstPermPosArc));
    EXPECT_FALSE(sets12IteratorBefore->Next());
  }

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerTarget(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure, targetTemplate};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);

  {
    auto const & sets12IteratorAfter = context.CreateIterator3(set1, ScType::ConstCommonArc, set2);
    EXPECT_TRUE(sets12IteratorAfter->Next());
    EXPECT_TRUE(context.CheckConnector(nrelInclusion, sets12IteratorAfter->Get(1), ScType::ConstPermPosArc));
    EXPECT_TRUE(context.CheckConnector(nrelSubset, sets12IteratorAfter->Get(1), ScType::ConstPermPosArc));
    EXPECT_FALSE(sets12IteratorAfter->Next());
  }
}

}  // namespace directInferenceManagerTest
