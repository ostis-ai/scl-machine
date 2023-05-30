/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_test.hpp"
#include "scs_loader.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/GenerationUtils.hpp"

#include "keynodes/InferenceKeynodes.hpp"
#include "factory/InferenceManagerFactory.hpp"

using namespace inference;

namespace inferenceManagerBuilderTest
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

using InferenceManagerBuilderTest = ScMemoryTest;

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

// Distributed input structures
TEST_F(InferenceManagerBuilderTest, SingleSuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");
  initialize();

  // Form input structures set of two structures. One of them consists one triple from premise and the other -- the
  // second triple.
  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrVector inputStructures{inputStructure1, inputStructure2};

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
  InferenceConfig const & inferenceConfig{
      GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
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

TEST_F(InferenceManagerBuilderTest, GenerateNotUnique)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");
  initialize();

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, {inputStructure1}, outputStructure};

  // GenerationType = GENERATE_ALL
  InferenceConfig const & inferenceConfig{
      GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
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
TEST_F(InferenceManagerBuilderTest, GenerateUnique)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateUniqueTest.scs");
  initialize();

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, {inputStructure1}, outputStructure};

  // GenerationType = GENERATE_UNIQUE
  InferenceConfig const & inferenceConfig{
      GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
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
}

// Test if all structures was generated by all arguments
TEST_F(InferenceManagerBuilderTest, GenerateNotFirst)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateNotFirstTest.scs");
  initialize();

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrVector inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddrVector arguments{argument};
  for (size_t i = 1; i < 6; i++)
  {
    arguments.push_back(context.HelperResolveSystemIdtf(ARGUMENT + to_string(i)));
  }
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig{
      GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
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
TEST_F(InferenceManagerBuilderTest, GenerateFirst)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "generateNotFirstTest.scs");
  initialize();

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrVector inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddrVector arguments{argument};
  for (size_t i = 1; i < 6; i++)
  {
    arguments.push_back(context.HelperResolveSystemIdtf(ARGUMENT + to_string(i)));
  }
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig{
      GENERATE_ALL_FORMULAS, REPLACEMENTS_FIRST, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
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

TEST_F(InferenceManagerBuilderTest, notGenerateSolutionTree)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");
  initialize();

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrVector inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, inputStructures, outputStructure};

  // Create inference manager with `strategy all` using director
  InferenceConfig const & inferenceConfig{
      GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
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

TEST_F(InferenceManagerBuilderTest, generateSolutionTree)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");
  initialize();

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrVector inputStructures{inputStructure1, inputStructure2};
  ScAddr const & argument = context.HelperResolveSystemIdtf(ARGUMENT);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);
  ScAddr const & formulasSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  InferenceParams const & inferenceParams{formulasSet, {argument}, inputStructures, outputStructure};

  // Create inference manager with `strategy all` using director
  InferenceConfig const & inferenceConfig{GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_FULL, SEARCH_IN_STRUCTURES};
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
TEST_F(InferenceManagerBuilderTest, SingleUnsuccessfulApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleUnsuccessfulApplyTest.scs");
  initialize();

  ScAddr const & inputStructure1 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE1);
  ScAddr const & inputStructure2 = context.HelperResolveSystemIdtf(INPUT_STRUCTURE2);
  ScAddrVector inputStructures{inputStructure1, inputStructure2};
  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(FORMULAS_SET);
  ScAddr const & outputStructure = context.CreateNode(ScType::NodeConstStruct);

  InferenceConfig const & inferenceConfig{
      GENERATE_ALL_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
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
}

}  // namespace inferenceManagerBuilderTest
