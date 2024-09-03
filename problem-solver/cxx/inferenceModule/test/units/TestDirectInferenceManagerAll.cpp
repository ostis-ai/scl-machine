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
  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};

  // Get arguments set. It is a singleton
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddrVector arguments{argument};

  // Create output structure to generate structures in
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  // Get formulas set to apply
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);

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

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);

  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));
}

TEST_P(InferenceManagerBuilderTest, GenerateNotUnique)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, {inputStructure1}, outputStructure};

  // GenerationType = GENERATE_ALL
  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_TRUE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));

  // Target class is created although it is exists. There are 2 target class edges
  ScIterator3Ptr const & targetClassIterator = context.Iterator3(targetClass, ScType::EdgeAccessConstPosPerm, argument);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());
}

// Logical formula is not generated because of generateOnlyUnique = true flag
TEST_P(InferenceManagerBuilderTest, GenerateUnique)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & argument2 = context.HelperResolveSystemIdtf(ARGUMENT + "2");
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {}, {inputStructure1, inputStructure2}, outputStructure};

  // GenerationType = GENERATE_UNIQUE
  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_FALSE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstNegPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr targetClassIterator = context.Iterator3(targetClass, ScType::EdgeAccessConstPosPerm, argument);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());
  targetClassIterator = context.Iterator3(targetClass, ScType::EdgeAccessConstPosPerm, argument2);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());

  ScIterator3Ptr const & outputStructureIterator =
      context.Iterator3(outputStructure, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  EXPECT_FALSE(outputStructureIterator->Next());
}

// Logical formula is not generated because of generateOnlyUnique = true flag
TEST_P(InferenceManagerBuilderTest, GenerateUniqueWithOutputStructure)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
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

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstNegPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr const & targetClassIterator = context.Iterator3(targetClass, ScType::EdgeAccessConstPosPerm, argument);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());

  ScIterator3Ptr const & outputStructureIterator =
      context.Iterator3(outputStructure, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
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

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddrVector arguments{argument};
  for (size_t i = 2; i < 6; i++)
  {
    arguments.push_back(context.HelperResolveSystemIdtf(ARGUMENT + to_string(i)));
  }
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, arguments, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  // Expect to generate 5 times
  ScAddr const & argumentsClass = context.HelperResolveSystemIdtf(CURRENT_NODE_CLASS);
  ScAddrVector const & expectedElements =
      utils::IteratorUtils::getAllWithType(&context, argumentsClass, ScType::NodeConst);
  ScAddrVector foundElements;
  ScIterator3Ptr const & targetClassIterator =
      context.Iterator3(targetClass, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
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

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddrVector arguments{argument};
  for (size_t i = 2; i < 6; i++)
  {
    arguments.push_back(context.HelperResolveSystemIdtf(ARGUMENT + to_string(i)));
  }
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, arguments, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  ScIterator3Ptr const & targetClassIterator =
      context.Iterator3(targetClass, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  EXPECT_TRUE(targetClassIterator->Next());
  EXPECT_FALSE(targetClassIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, notGenerateSolutionTree)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, inputStructures, outputStructure};

  // Create inference manager with `strategy all` using director
  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  // Apply inference with configured manager and params config
  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_TRUE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & solutionNode = utils::IteratorUtils::getAnyFromSet(&context, solution);
  EXPECT_FALSE(solutionNode.IsValid());

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));
}

TEST_P(InferenceManagerBuilderTest, generateSolutionTree)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, inputStructures, outputStructure};

  // Create inference manager with `strategy all` using director
  InferenceConfig const & inferenceConfig =
      GetParam()->getInferenceConfig({GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_FULL, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  // Apply inference with configured manager and params config
  bool result = iterationStrategy->applyInference(inferenceParams);
  EXPECT_TRUE(result);

  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & solutionNode = utils::IteratorUtils::getAnyFromSet(&context, solution);
  EXPECT_TRUE(solutionNode.IsValid());

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));
}

// There are no arguments using fixed formulas arguments -- not generated
TEST_P(InferenceManagerBuilderTest, SingleUnsuccessfulApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleUnsuccessfulApplyTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_FALSE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstNegPerm));

  ScAddr const & argument = context.HelperFindBySystemIdtf(ARGUMENT);
  EXPECT_TRUE(argument.IsValid());
  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  EXPECT_FALSE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));

  auto iterator = context.Iterator3(outputStructure, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  EXPECT_FALSE(iterator->Next());
}

TEST_P(InferenceManagerBuilderTest, SingleSuccessfulApplyInferenceWithAccessEdgesSearcher)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyAccessEdgesTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_ONLY_ACCESS_EDGES_IN_STRUCTURES});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & argument = context.HelperFindBySystemIdtf(ARGUMENT);
  EXPECT_TRUE(argument.IsValid());
  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));
}

TEST_P(InferenceManagerBuilderTest, OutputStructureContainsGeneratedAndFlagIsGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_ACCESS_EDGES_IN_STRUCTURES,
       GENERATED_ONLY});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(outputStructure, targetClass, ScType::EdgeAccessConstPosPerm));
  ScIterator3Ptr const & outputStructureIterator =
      context.Iterator3(outputStructure, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_FALSE(outputStructureIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, OutputStructureContainsGeneratedButFlagIsSearchedAndGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_ALL_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_ACCESS_EDGES_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(outputStructure, targetClass, ScType::EdgeAccessConstPosPerm));
  ScIterator3Ptr const & outputStructureIterator =
      context.Iterator3(outputStructure, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_FALSE(outputStructureIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, OutputStructureContainsSearchedAndFlagIsSearchedAndGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddrUnorderedSet inputStructures{inputStructure1};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_ACCESS_EDGES_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_FALSE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstNegPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(outputStructure, targetClass, ScType::EdgeAccessConstPosPerm));
  ScIterator3Ptr const & outputStructureIterator =
      context.Iterator3(outputStructure, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_TRUE(outputStructureIterator->Next());
  EXPECT_FALSE(outputStructureIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, OutputStructureDoesNotContainSearchedBecauseFlagIsGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddrUnorderedSet inputStructures{inputStructure1};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_ACCESS_EDGES_IN_STRUCTURES,
       GENERATED_ONLY});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_FALSE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstNegPerm));

  ScIterator3Ptr const & outputStructureIterator =
      context.Iterator3(outputStructure, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  EXPECT_FALSE(outputStructureIterator->Next());
}

TEST_P(InferenceManagerBuilderTest, OutputStructureContainsSearchedAndGeneratedAndFlagIsSearchedAndGenerated)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateNotFirstTest.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & argument = context.HelperFindBySystemIdtf(ARGUMENT);
  EXPECT_TRUE(argument.IsValid());
  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());
  EXPECT_TRUE(context.CreateEdge(ScType::EdgeAccessConstPosPerm, targetClass, argument).IsValid());

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_ACCESS_EDGES_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);
  ScAddr const & solution = iterationStrategy->getSolutionTreeManager()->createSolution(outputStructure, result);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & currentClass = context.HelperFindBySystemIdtf(CURRENT_NODE_CLASS);
  EXPECT_TRUE(currentClass.IsValid());
  size_t const currentClassSize =
      utils::IteratorUtils::getAllWithType(&context, currentClass, ScType::NodeConst).size();
  EXPECT_NE(currentClassSize, 0u);
  size_t const targetClassSize = utils::IteratorUtils::getAllWithType(&context, targetClass, ScType::NodeConst).size();
  EXPECT_EQ(currentClassSize, targetClassSize);
  EXPECT_TRUE(context.HelperCheckEdge(outputStructure, targetClass, ScType::EdgeAccessConstPosPerm));
  ScIterator3Ptr const & targetClassIterator =
      context.Iterator3(targetClass, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  while (targetClassIterator->Next())
  {
    EXPECT_TRUE(context.HelperCheckEdge(outputStructure, targetClassIterator->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_TRUE(context.HelperCheckEdge(outputStructure, targetClassIterator->Get(2), ScType::EdgeAccessConstPosPerm));
  }
}

TEST_P(InferenceManagerBuilderTest, conclusionContainsEdgeReplacementFromPremise)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "conclusionContainsEdgeReplacementFromPremise.scs");

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrUnorderedSet inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & set1 = context.HelperFindBySystemIdtf("set1");
  ScAddr const & set2 = context.HelperFindBySystemIdtf("set2");
  ScAddr const & set3 = context.HelperFindBySystemIdtf("set3");
  EXPECT_TRUE(set1.IsValid());
  EXPECT_TRUE(set2.IsValid());
  EXPECT_TRUE(set3.IsValid());
  ScAddr const & nrelInclusion = context.HelperFindBySystemIdtf("nrel_inclusion");
  ScAddr const & nrelSubset = context.HelperFindBySystemIdtf("nrel_subset");
  EXPECT_TRUE(nrelInclusion.IsValid());
  EXPECT_TRUE(nrelSubset.IsValid());
  {
    auto const & sets12IteratorBefore = context.Iterator3(set1, ScType::EdgeDCommonConst, set2);
    EXPECT_TRUE(sets12IteratorBefore->Next());
    EXPECT_TRUE(context.HelperCheckEdge(nrelInclusion, sets12IteratorBefore->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_FALSE(context.HelperCheckEdge(nrelSubset, sets12IteratorBefore->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_FALSE(sets12IteratorBefore->Next());
    auto const & sets23IteratorBefore = context.Iterator3(set2, ScType::EdgeDCommonConst, set3);
    EXPECT_TRUE(sets23IteratorBefore->Next());
    EXPECT_TRUE(context.HelperCheckEdge(nrelInclusion, sets23IteratorBefore->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_TRUE(context.HelperCheckEdge(nrelSubset, sets23IteratorBefore->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_FALSE(sets23IteratorBefore->Next());
  }

  InferenceConfig const & inferenceConfig = GetParam()->getInferenceConfig(
      {GENERATE_UNIQUE_FORMULAS,
       REPLACEMENTS_ALL,
       TREE_ONLY_OUTPUT_STRUCTURE,
       SEARCH_ONLY_ACCESS_EDGES_IN_STRUCTURES,
       SEARCHED_AND_GENERATED});
  std::unique_ptr<inference::InferenceManagerAbstract> iterationStrategy =
      inference::InferenceManagerFactory::constructDirectInferenceManagerAll(&context, inferenceConfig);

  InferenceParams const & inferenceParams{rulesSet, {}, inputStructures, outputStructure};
  bool result = iterationStrategy->applyInference(inferenceParams);

  EXPECT_TRUE(result);

  {
    auto const & sets12IteratorAfter = context.Iterator3(set1, ScType::EdgeDCommonConst, set2);
    EXPECT_TRUE(sets12IteratorAfter->Next());
    EXPECT_TRUE(context.HelperCheckEdge(nrelInclusion, sets12IteratorAfter->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_TRUE(context.HelperCheckEdge(nrelSubset, sets12IteratorAfter->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_FALSE(sets12IteratorAfter->Next());
    auto const & sets23IteratorAfter = context.Iterator3(set2, ScType::EdgeDCommonConst, set3);
    EXPECT_TRUE(sets23IteratorAfter->Next());
    EXPECT_TRUE(context.HelperCheckEdge(nrelInclusion, sets23IteratorAfter->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_TRUE(context.HelperCheckEdge(nrelSubset, sets23IteratorAfter->Get(1), ScType::EdgeAccessConstPosPerm));
    EXPECT_FALSE(sets23IteratorAfter->Next());
  }
}

}  // namespace inference::inferenceManagerBuilderTest
