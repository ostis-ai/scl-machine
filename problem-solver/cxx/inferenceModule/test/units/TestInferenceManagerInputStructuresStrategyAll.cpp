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
#include "director/InferenceManagerDirector.hpp"
#include "builder/InferenceManagerInputStructuresBuilder.hpp"

using namespace inference;

namespace inferenceManagerBuilderTest
{
ScsLoader loader;
std::string const TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/InferenceManagerInputStructuresStrategyAll/";

std::string const INPUT_STRUCTURES = "input_structures";
std::string const RULES_SET = "rules_set";
std::string const ARGUMENT = "argument";
std::string const ARGUMENTS = "arguments";
std::string const TARGET_NODE_CLASS = "target_node_class";
std::string const CURRENT_NODE_CLASS = "current_node_class";

using InferenceManagerBuilderTest = ScMemoryTest;

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

void generateTripleInStructure(ScMemoryContext & context, ScAddr const & set, ScAddr const & element, ScAddr const & structure)
{
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, element);
  ScAddr const & tailElementArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, set, element);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, tailElementArc);
}

void generateBodyHeadInStructure(ScMemoryContext & context, ScAddr const & tailElement, ScAddr const & nrelBodyElements, ScAddr const & nrelHead, ScAddr const & structure)
{
  ScAddr const & bodyElementsTuple = context.CreateNode(ScType::NodeConstTuple);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, bodyElementsTuple);

  ScAddr const & bodyElementsEdge = context.CreateEdge(ScType::EdgeDCommonConst, tailElement, bodyElementsTuple);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, bodyElementsEdge);

  ScAddr const & bodyElementsArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelBodyElements, bodyElementsEdge);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, bodyElementsArc);

  ScAddr const & bodyElement = context.CreateNode(ScType::NodeConst);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, bodyElement);

  ScAddr const & bodyElementArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, bodyElementsTuple, bodyElement);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, bodyElementArc);

  ScAddr const & head = context.CreateNode(ScType::NodeConst);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, head);

  ScAddr const & headEdge = context.CreateEdge(ScType::EdgeDCommonConst, bodyElement, head);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, headEdge);

  ScAddr const & headArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelHead, headEdge);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, structure, headArc);
}

void generateSnakes(ScMemoryContext & context, size_t const snakesCount, size_t const tailsElementsCount)
{
  ScAddr const & inputStructure = context.HelperFindBySystemIdtf("input_structure1");
  EXPECT_TRUE(inputStructure.IsValid());
  ScAddr const & classTail = context.HelperFindBySystemIdtf("class_tail");
  EXPECT_TRUE(classTail.IsValid());
  ScAddr const & nrelTailElements = context.HelperFindBySystemIdtf("nrel_tail_elements");
  EXPECT_TRUE(nrelTailElements.IsValid());
  ScAddr const & nrelBodyElements = context.HelperFindBySystemIdtf("nrel_body_elements");
  EXPECT_TRUE(nrelBodyElements.IsValid());
  ScAddr const & nrelHead = context.HelperFindBySystemIdtf("nrel_head");
  EXPECT_TRUE(nrelHead.IsValid());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, classTail);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, nrelTailElements);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, nrelBodyElements);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, nrelHead);

  for (size_t i = 0; i < snakesCount; i++)
  {
    ScAddr tailElement;
    ScAddr const & tail = context.CreateNode(ScType::NodeConst);
    context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, tail);
    ScAddr const & classEdge = context.CreateEdge(ScType::EdgeAccessConstPosPerm, classTail, tail);
    context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, classEdge);

    ScAddr const & tailElementsTuple = context.CreateNode(ScType::NodeConstTuple);
    context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, tailElementsTuple);

    ScAddr const & tailElementsEdge = context.CreateEdge(ScType::EdgeDCommonConst, tail, tailElementsTuple);
    context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, tailElementsEdge);

    ScAddr const & tailElementsArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelTailElements, tailElementsEdge);
    context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, tailElementsArc);

    for (size_t j = 0; j < tailsElementsCount; j++)
    {
      tailElement = context.CreateNode(ScType::NodeConst);
      generateTripleInStructure(context, tailElementsTuple, tailElement, inputStructure);
    }

    generateBodyHeadInStructure(context, tailElement, nrelBodyElements, nrelHead, inputStructure);
  }
}

void generateTails(ScMemoryContext & context, ScAddr const & tail, size_t const tailsCount, size_t const tailsElementsCount)
{
  ScAddr const & inputStructure = context.HelperFindBySystemIdtf("input_structure1");
  EXPECT_TRUE(inputStructure.IsValid());
  ScAddr const & classTail = context.HelperFindBySystemIdtf("class_tail");
  EXPECT_TRUE(classTail.IsValid());
  ScAddr const & nrelTailElements = context.HelperFindBySystemIdtf("nrel_tail_elements");
  EXPECT_TRUE(nrelTailElements.IsValid());
  ScAddr const & nrelBodyElements = context.HelperFindBySystemIdtf("nrel_body_elements");
  EXPECT_TRUE(nrelBodyElements.IsValid());
  ScAddr const & nrelHead = context.HelperFindBySystemIdtf("nrel_head");
  EXPECT_TRUE(nrelHead.IsValid());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, tail);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, classTail);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, nrelTailElements);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, nrelBodyElements);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, nrelHead);

  ScAddr const & classEdge = context.CreateEdge(ScType::EdgeAccessConstPosPerm, classTail, tail);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, classEdge);

  ScAddr const & tailElementsTuple = context.CreateNode(ScType::NodeConst);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, tailElementsTuple);

  ScAddr const & tailElementsEdge = context.CreateEdge(ScType::EdgeDCommonConst, tail, tailElementsTuple);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, tailElementsEdge);

  ScAddr const & tailElementsArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelTailElements, tailElementsEdge);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, inputStructure, tailElementsArc);

  ScAddr tailElement;
  for (size_t i = 0; i < tailsCount; i++)
  {
    for (size_t j = 0; j < tailsElementsCount; j++)
    {
      tailElement = context.CreateNode(ScType::NodeConst);
      generateTripleInStructure(context, tailElementsTuple, tailElement, inputStructure);
    }

    generateBodyHeadInStructure(context, tailElement, nrelBodyElements, nrelHead, inputStructure);
  }
}

TEST_F(InferenceManagerBuilderTest, SingleSuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");
  initialize();

  // Get input structures set of two structures. One of them consists one triple from premise and the other -- the second triple.
  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);
  // Get arguments set. It is a singleton
  ScAddr const & arguments = context.HelperResolveSystemIdtf(ARGUMENTS);

  // Create inference builder with input structures
  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures, arguments);

  // Create inference manager with `strategy all` using director with builder
  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);

  // Apply inference with configured manager
  ScAddr const & solution = inferenceManager->applyInference(rulesSet);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & argument = context.HelperFindBySystemIdtf(ARGUMENT);
  EXPECT_TRUE(argument.IsValid());
  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));
}

TEST_F(InferenceManagerBuilderTest, CycleSingleSuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");
  initialize();

  for (size_t i = 0; i < 1078; i++)
  {
    // Get input structures set of two structures. One of them consists one triple from premise and the other -- the second triple.
    ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);
    // Get arguments set. It is a singleton
    ScAddr const & arguments = context.HelperResolveSystemIdtf(ARGUMENTS);

    // Create inference builder with input structures
    std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
          std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures, arguments);

    // Create inference manager with `strategy all` using director with builder
    std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
          inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

    ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);

    // Apply inference with configured manager
    ScAddr const & solution = inferenceManager->applyInference(rulesSet);

    EXPECT_TRUE(solution.IsValid());
    EXPECT_TRUE(
          context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

    ScAddr const & argument = context.HelperFindBySystemIdtf(ARGUMENT);
    EXPECT_TRUE(argument.IsValid());
    ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
    EXPECT_TRUE(targetClass.IsValid());

    EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));
  }
}

TEST_F(InferenceManagerBuilderTest, SingleSuccessArgumentApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleArgumentApplyTest.scs");
  initialize();

  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);
  ScAddr const & arguments = context.HelperResolveSystemIdtf(ARGUMENTS);

  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures, arguments);

  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);

  ScAddr const & solution = inferenceManager->applyInference(rulesSet);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & argument = context.HelperFindBySystemIdtf(ARGUMENT);
  EXPECT_TRUE(argument.IsValid());
  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  EXPECT_TRUE(context.HelperCheckEdge(targetClass, argument, ScType::EdgeAccessConstPosPerm));
}

TEST_F(InferenceManagerBuilderTest, SnakeApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "snakeSingleArgumentApplyTest.scs");
  initialize();

  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);
  ScAddr const & arguments = context.HelperResolveSystemIdtf(ARGUMENTS);

  auto start_time = std::chrono::high_resolution_clock::now();
  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures, arguments);

  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);
  ScAddr const & solution = inferenceManager->applyInference(rulesSet);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto time = end_time - start_time;
  SC_LOG_WARNING("Apply inference milliseconds: " << time/std::chrono::milliseconds(1));

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & tail = context.HelperFindBySystemIdtf("tail");
  ScAddr const & head = context.HelperFindBySystemIdtf("head");
  ScAddr const & nrelTailHead = context.HelperFindBySystemIdtf("nrel_tail_head");
  ScIterator5Ptr const & conclusionIterator = context.Iterator5(
        tail,
        ScType::EdgeDCommonConst,
        head,
        ScType::EdgeAccessConstPosPerm,
        nrelTailHead);

  EXPECT_TRUE(conclusionIterator->Next());
}

TEST_F(InferenceManagerBuilderTest, SnakesApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "snakeSingleArgumentApplyTest.scs");
  initialize();

  generateSnakes(*m_ctx, 1000, 100);

  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);
  ScAddr const & arguments = context.HelperResolveSystemIdtf(ARGUMENTS);

  auto start_time = std::chrono::high_resolution_clock::now();
  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures, arguments);

  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);
  ScAddr const & solution = inferenceManager->applyInference(rulesSet);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto time = end_time - start_time;
  SC_LOG_WARNING("Apply inference milliseconds: " << time/std::chrono::milliseconds(1));

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & tail = context.HelperFindBySystemIdtf("tail");
  ScAddr const & head = context.HelperFindBySystemIdtf("head");
  ScAddr const & nrelTailHead = context.HelperFindBySystemIdtf("nrel_tail_head");
  ScIterator5Ptr const & conclusionIterator = context.Iterator5(
        tail,
        ScType::EdgeDCommonConst,
        head,
        ScType::EdgeAccessConstPosPerm,
        nrelTailHead);

  EXPECT_TRUE(conclusionIterator->Next());
}

TEST_F(InferenceManagerBuilderTest, SnakesTailsApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "snakeSingleArgumentApplyTest.scs");
  initialize();

  ScAddr const & tail = context.HelperFindBySystemIdtf("tail");
  generateTails(*m_ctx, tail, 1, 10000);

  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);
  ScAddr const & arguments = context.HelperResolveSystemIdtf(ARGUMENTS);

  auto start_time = std::chrono::high_resolution_clock::now();
  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures, arguments);

  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);
  ScAddr const & solution = inferenceManager->applyInference(rulesSet);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto time = end_time - start_time;
  SC_LOG_WARNING("Apply inference milliseconds: " << time/std::chrono::milliseconds(1));

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));
}

TEST_F(InferenceManagerBuilderTest, SnakesTailsConjunctionApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "snakeSingleArgumentConjunctionApplyTest.scs");
  initialize();

  ScAddr const & tail = context.HelperFindBySystemIdtf("tail");
  generateTails(*m_ctx, tail, 10, 10000);

  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);
  ScAddr const & arguments = context.HelperResolveSystemIdtf(ARGUMENTS);

  auto start_time = std::chrono::high_resolution_clock::now();
  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures, arguments);

  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);
  ScAddr const & solution = inferenceManager->applyInference(rulesSet);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto time = end_time - start_time;
  SC_LOG_WARNING("Apply inference milliseconds: " << time/std::chrono::milliseconds(1));

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));
}

TEST_F(InferenceManagerBuilderTest, MultipleSuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "fiveTimesApplyTest.scs");
  initialize();

  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);

  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures);

  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);

  ScAddr const & solution = inferenceManager->applyInference(rulesSet);

  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstPosPerm));

  ScAddr const & targetClass = context.HelperFindBySystemIdtf(TARGET_NODE_CLASS);
  EXPECT_TRUE(targetClass.IsValid());

  // Test if all structures was generated during `inference strategy all` flow
  ScAddr const & argumentsClass = context.HelperResolveSystemIdtf(CURRENT_NODE_CLASS);
  ScAddrVector const & expectedElements = utils::IteratorUtils::getAllWithType(&context, argumentsClass, ScType::NodeConst);
  ScAddrVector foundElements;
  ScIterator3Ptr const & targetClassIterator = context.Iterator3(
        targetClass,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst);
  for (size_t count = 0; count < 5; count++)
  {
    EXPECT_TRUE(targetClassIterator->Next());
    foundElements.push_back(targetClassIterator->Get(2));
  }

  EXPECT_EQ(foundElements.size(), expectedElements.size());
}

TEST_F(InferenceManagerBuilderTest, SingleUnsuccessfulApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleUnsuccessfulApplyTest.scs");
  initialize();

  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);

  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures);

  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresFixedArgumentsStrategyAll(&context, std::move(builder));

  ScAddr const & rulesSet = context.HelperResolveSystemIdtf(RULES_SET);

  ScAddr const & solution = inferenceManager->applyInference(rulesSet);

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
