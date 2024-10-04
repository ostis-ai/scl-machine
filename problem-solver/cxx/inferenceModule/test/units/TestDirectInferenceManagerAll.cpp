/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ConfigGenerators.hpp"

#include "factory/InferenceManagerFactory.hpp"

#include "keynodes/InferenceKeynodes.hpp"

#include <sc_test.hpp>
#include <scs_loader.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>

namespace inference::inferenceManagerBuilderTest
{
ScsLoader loader;
std::string const TEST_FILES_DIR_PATH =
    TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/DirectInferenceManagerAll/";

std::string const INPUT_STRUCTURE1 = "input_structure1";
std::string const INPUT_STRUCTURE2 = "input_structure2";
std::string const FORMULAS_SET = "formulas_set";
std::string const ARGUMENT = "argument";
std::string const TAIL = "tail";
std::string const TARGET_NODE_CLASS = "target_node_class";
std::string const CURRENT_NODE_CLASS = "current_node_class";

class InferenceManagerBuilderTest
  : public ScMemoryTest
  , public testing::WithParamInterface<std::shared_ptr<generatorTest::ConfigGenerator>>
{
};

std::shared_ptr<generatorTest::ConfigGenerator> generators[] = {
    std::make_shared<generatorTest::ConfigGenerator>(),
    std::make_shared<generatorTest::ConfigGeneratorSearchWithReplacements>(),
    std::make_shared<generatorTest::ConfigGeneratorSearchWithoutReplacements>()};

INSTANTIATE_TEST_SUITE_P(
    InferenceManagerBuilderTestInitiator,
    InferenceManagerBuilderTest,
    testing::ValuesIn(generators),
    [](testing::TestParamInfo<std::shared_ptr<generatorTest::ConfigGenerator>> const & testParamInfo) {
      return testParamInfo.param->getName();
    });

// Distributed input structures
TEST_P(InferenceManagerBuilderTest, SingleSuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  // Form input structures set of two structures. One of them consists one triple from premise and the other -- the
  // second triple.
  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};

  // Get arguments set. It is a singleton
  ScAddr const & argument = context.ResolveElementSystemIdentifier(ARGUMENT);
  ScAddrVector arguments{argument};

  // Create output structure to generate structures in
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  // Get formulas set to apply
  ScAddr const & formulasSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);

  // Form inference params config
  InferenceParams const & inferenceParams{formulasSet, arguments, inputStructures, outputStructure};

  // Create inference manager with `strategy all` using director
  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  // Apply inference with configured manager and params config
  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_TRUE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);

  EXPECT_TRUE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));
}

TEST_P(InferenceManagerBuilderTest, GenerateNotUnique)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & argument = context.ResolveElementSystemIdentifier(ARGUMENT);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & formulasSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, {inputStructure1}, outputStructure};

  // GenerationType = GENERATE_ALL
  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_TRUE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));

  // Target class is created although it exists. There are 2 target class membership arcs
  ScIterator3Ptr const & targetClassIterator =
      context.CreateIterator3(targetClass, ScType::ConstPermPosArc, argument);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());
}

// Logical formula is not generated because of generateOnlyUnique = true flag
TEST_P(InferenceManagerBuilderTest, GenerateUnique)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddr const & argument = context.ResolveElementSystemIdentifier(ARGUMENT);
  ScAddr const & argument2 = context.ResolveElementSystemIdentifier(ARGUMENT + "2");
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & formulasSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {}, {inputStructure1, inputStructure2}, outputStructure};

  // GenerationType = GENERATE_UNIQUE
  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_FALSE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermNegArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));

  ScIterator3Ptr targetClassIterator = context.CreateIterator3(targetClass, ScType::ConstPermPosArc, argument);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());
  targetClassIterator = context.CreateIterator3(targetClass, ScType::ConstPermPosArc, argument2);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());

  ScIterator3Ptr const & outputStructureIterator =
      context.CreateIterator3(outputStructure, ScType::ConstPermPosArc, ScType::Unknown);
  EXPECT_FALSE(outputStructureIterator->Next());
}

// Logical formula is not generated because of generateOnlyUnique = true flag
TEST_P(InferenceManagerBuilderTest, GenerateUniqueWithOutputStructure)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & argument = context.ResolveElementSystemIdentifier(ARGUMENT);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & formulasSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, {inputStructure1}, outputStructure};

  // GenerationType = GENERATE_UNIQUE
  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_FALSE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermNegArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));

  ScIterator3Ptr const & targetClassIterator =
      context.CreateIterator3(targetClass, ScType::ConstPermPosArc, argument);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());

  ScIterator3Ptr const & outputStructureIterator =
      context.CreateIterator3(outputStructure, ScType::ConstPermPosArc, ScType::Unknown);
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_FALSE(outputStructureIterator->Next());
}

// Test if all structures was generated by all arguments
TEST_P(InferenceManagerBuilderTest, GenerateNotFirst)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateNotFirstTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.ResolveElementSystemIdentifier(ARGUMENT);
  ScAddrVector arguments{argument};
  for (size_t i = 2; i < 6; i++)
  {
    arguments.push_back(context.ResolveElementSystemIdentifier(ARGUMENT + to_string(i)));
  }
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, arguments, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  // Expect to generate 5 times
  ScAddr const & argumentsClass = context.ResolveElementSystemIdentifier(CURRENT_NODE_CLASS);
  ScAddrVector const & expectedElements =
      utils::IteratorUtils::getAllWithType(&context, argumentsClass, ScType::ConstNode);
  ScAddrVector foundElements;
  ScIterator3Ptr const & targetClassIterator =
      context.CreateIterator3(targetClass, ScType::ConstPermPosArc, ScType::ConstNode);
  for (size_t count = 0; count < 5; count++)
  {
    EXPECT_TRUE(targetClassIterator->Next());
    foundElements.push_back(targetClassIterator->Get(2));
  }

  EXPECT_EQ(foundElements.size(), expectedElements.size());
}

// Test if only first structure was generated by arguments
TEST_P(InferenceManagerBuilderTest, GenerateFirst)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateNotFirstTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.ResolveElementSystemIdentifier(ARGUMENT);
  ScAddrVector arguments{argument};
  for (size_t i = 2; i < 6; i++)
  {
    arguments.push_back(context.ResolveElementSystemIdentifier(ARGUMENT + to_string(i)));
  }
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, arguments, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  ScIterator3Ptr const & targetClassIterator =
      context.CreateIterator3(targetClass, ScType::ConstPermPosArc, ScType::ConstNode);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, notGenerateSolutionTree)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.ResolveElementSystemIdentifier(ARGUMENT);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & formulasSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, inputStructures, outputStructure};

  // Create inference manager with `strategy all` using director
  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  // Apply inference with configured manager and params config
  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_TRUE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & solutionNode = utils::IteratorUtils::getAnyFromSet(&context, solution);
  EXPECT_FALSE(solutionNode.IsValid());

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));
}

TEST_P(InferenceManagerBuilderTest, generateSolutionTree)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.ResolveElementSystemIdentifier(ARGUMENT);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & formulasSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, inputStructures, outputStructure};

  // Create inference manager with `strategy all` using director
  InferenceConfig const & inferenceConfig =
      GetParam()->getInferenceConfig({GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_FULL, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  // Apply inference with configured manager and params config
  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_TRUE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & solutionNode = utils::IteratorUtils::getAnyFromSet(&context, solution);
  EXPECT_TRUE(solutionNode.IsValid());

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));
}

// There are no arguments using fixed formulas arguments -- not generated
TEST_P(InferenceManagerBuilderTest, SingleUnsuccessfulApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleUnsuccessfulApplyTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_FALSE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermNegArc));

  ScAddr const & argument = context.SearchElementBySystemIdentifier(ARGUMENT);
  EXPECT_TRUE(argument.IsValid());
  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  EXPECT_FALSE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));

  auto iterator = context.CreateIterator3(outputStructure, ScType::ConstPermPosArc, ScType::Unknown);
  EXPECT_FALSE(iterator->Next());
}

TEST_P(InferenceManagerBuilderTest, SingleSuccessfulApplyInferenceWithMembershipArcsSearcher)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyMembershipArcsTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & argument = context.SearchElementBySystemIdentifier(ARGUMENT);
  EXPECT_TRUE(argument.IsValid());
  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  EXPECT_TRUE(context.CheckConnector(targetClass, argument, ScType::ConstPermPosArc));
}

TEST_P(InferenceManagerBuilderTest, OutputStructureContainsGeneratedAndFlagIsGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES,
       GENERATED_ONLY});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(context.CheckConnector(outputStructure, targetClass, ScType::ConstPermPosArc));
  ScIterator3Ptr const & outputStructureIterator =
      context.CreateIterator3(outputStructure, ScType::ConstPermPosArc, ScType::Unknown);
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_FALSE(outputStructureIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, OutputStructureContainsGeneratedButFlagIsSearchedAndGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(context.CheckConnector(outputStructure, targetClass, ScType::ConstPermPosArc));
  ScIterator3Ptr const & outputStructureIterator =
      context.CreateIterator3(outputStructure, ScType::ConstPermPosArc, ScType::Unknown);
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_FALSE(outputStructureIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, OutputStructureContainsSearchedAndFlagIsSearchedAndGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddrUnorderedSet inputStructures{inputStructure1};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_FALSE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermNegArc));

  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(context.CheckConnector(outputStructure, targetClass, ScType::ConstPermPosArc));
  ScIterator3Ptr const & outputStructureIterator =
      context.CreateIterator3(outputStructure, ScType::ConstPermPosArc, ScType::Unknown);
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_FALSE(outputStructureIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, OutputStructureDoesNotContainSearchedBecauseFlagIsGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddrUnorderedSet inputStructures{inputStructure1};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES,
       GENERATED_ONLY});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_FALSE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermNegArc));

  ScIterator3Ptr const & outputStructureIterator =
      context.CreateIterator3(outputStructure, ScType::ConstPermPosArc, ScType::Unknown);
  EXPECT_FALSE(outputStructureIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, OutputStructureContainsSearchedAndGeneratedAndFlagIsSearchedAndGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateNotFirstTest.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & argument = context.SearchElementBySystemIdentifier(ARGUMENT);
  EXPECT_TRUE(argument.IsValid());
  ScAddr const & targetClass = context.SearchElementBySystemIdentifier(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(context.GenerateConnector(ScType::ConstPermPosArc, targetClass, argument).IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->generateSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermPosArc));

  ScAddr const & currentClass = context.SearchElementBySystemIdentifier(CURRENT_NODE_CLASS);
  EXPECT_TRUE(currentClass.IsValid());
  size_t const currentClassSize =
      utils::IteratorUtils::getAllWithType(&context, currentClass, ScType::ConstNode).size();
  EXPECT_NE(currentClassSize, 0u);
  size_t const targetClassSize = utils::IteratorUtils::getAllWithType(&context, targetClass, ScType::ConstNode).size();
  EXPECT_EQ(currentClassSize, targetClassSize);
  EXPECT_TRUE(context.CheckConnector(outputStructure, targetClass, ScType::ConstPermPosArc));
  ScIterator3Ptr const & targetClassIterator =
      context.CreateIterator3(targetClass, ScType::ConstPermPosArc, ScType::Unknown);
  while (targetClassIterator->Next())
  {
    EXPECT_TRUE(context.CheckConnector(outputStructure, targetClassIterator->Get(1), ScType::ConstPermPosArc));
    EXPECT_TRUE(context.CheckConnector(outputStructure, targetClassIterator->Get(2), ScType::ConstPermPosArc));
  }
}

TEST_P(InferenceManagerBuilderTest, conclusionContainsArcReplacementFromPremise)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "conclusionContainsArcReplacementFromPremise.scs");

  ScAddr const & inputStructure1 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.ResolveElementSystemIdentifier(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.ResolveElementSystemIdentifier(FORMULAS_SET);
  ScAddr const & outputStructure = context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & set1 = context.SearchElementBySystemIdentifier("set1");
  ScAddr const & set2 = context.SearchElementBySystemIdentifier("set2");
  ScAddr const & set3 = context.SearchElementBySystemIdentifier("set3");
  EXPECT_TRUE(set1.IsValid());
  EXPECT_TRUE(set2.IsValid());
  EXPECT_TRUE(set3.IsValid());
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
    auto const & sets23IteratorBefore = context.CreateIterator3(set2, ScType::ConstCommonArc, set3);
    EXPECT_TRUE(sets23IteratorBefore->Next());
    EXPECT_TRUE(context.CheckConnector(nrelInclusion, sets23IteratorBefore->Get(1), ScType::ConstPermPosArc));
    EXPECT_TRUE(context.CheckConnector(nrelSubset, sets23IteratorBefore->Get(1), ScType::ConstPermPosArc));
    EXPECT_FALSE(sets23IteratorBefore->Next());
  }

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);

  {
    auto const & sets12IteratorAfter = context.CreateIterator3(set1, ScType::ConstCommonArc, set2);
    EXPECT_TRUE(sets12IteratorAfter->Next());
    EXPECT_TRUE(context.CheckConnector(nrelInclusion, sets12IteratorAfter->Get(1), ScType::ConstPermPosArc));
    EXPECT_TRUE(context.CheckConnector(nrelSubset, sets12IteratorAfter->Get(1), ScType::ConstPermPosArc));
    EXPECT_FALSE(sets12IteratorAfter->Next());
    auto const & sets23IteratorAfter = context.CreateIterator3(set2, ScType::ConstCommonArc, set3);
    EXPECT_TRUE(sets23IteratorAfter->Next());
    EXPECT_TRUE(context.CheckConnector(nrelInclusion, sets23IteratorAfter->Get(1), ScType::ConstPermPosArc));
    EXPECT_TRUE(context.CheckConnector(nrelSubset, sets23IteratorAfter->Get(1), ScType::ConstPermPosArc));
    EXPECT_FALSE(sets23IteratorAfter->Next());
  }
}

}  // namespace inference::inferenceManagerBuilderTest
