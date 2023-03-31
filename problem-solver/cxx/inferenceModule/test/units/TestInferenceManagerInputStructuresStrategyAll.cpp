/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_test.hpp"
#include "scs_loader.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

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

TEST_F(InferenceManagerBuilderTest, SingleSuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleApplyTest.scs");
  initialize();

  // Get input structures set of two structures. One of them consists one triple from premise and the other -- the second triple.
  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);

  // Create inference builder with input structures
  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures);

  // Create inference manager with `strategy all` using director with builder
  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresStrategyAll(&context, std::move(builder));

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

TEST_F(InferenceManagerBuilderTest, SingleSuccessArgumentApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "singleArgumentApplyTest.scs");
  initialize();

  // Get input structures set of two structures. One of them consists one triple from premise and the other -- the second triple.
  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);

  ScAddr const & arguments = context.HelperResolveSystemIdtf(ARGUMENTS);

  // Create inference builder with input structures
  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures, arguments);

  // Create inference manager with `strategy all` using director with builder
  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresStrategyAll(&context, std::move(builder));

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

TEST_F(InferenceManagerBuilderTest, MultipleSuccessApplyInference)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "fiveTimesApplyTest.scs");
  initialize();

  ScAddr const & inputStructures = context.HelperResolveSystemIdtf(INPUT_STRUCTURES);

  std::unique_ptr<inference::InferenceManagerBuilderAbstract> builder =
        std::make_unique<inference::InferenceManagerInputStructuresBuilder>(&context, inputStructures);

  std::unique_ptr<inference::InferenceManagerGeneral> inferenceManager =
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresStrategyAll(&context, std::move(builder));

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

  // TODO: incorrect comparing
  EXPECT_EQ(foundElements, expectedElements);
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
        inference::InferenceManagerDirector::constructDirectInferenceManagerInputStructuresStrategyAll(&context, std::move(builder));

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
