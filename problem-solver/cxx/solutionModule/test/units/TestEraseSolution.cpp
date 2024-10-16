/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "agent/EraseSolutionAgent.hpp"

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>

namespace eraseSolutionAgentTest
{
ScsLoader loader;
const std::string ERASE_SOLUTION_MODULE_TEST_FILES_DIR_PATH =
    SOLUTION_MODULE_TEST_SRC_PATH "/testStructures/eraseSolution/";
const int WAIT_TIME = 5000;

using EraseSolutionAgentTest = ScMemoryTest;

void initialize(ScAgentContext & context)
{
  context.SubscribeAgent<solutionModule::EraseSolutionAgent>();
}

void shutdown(ScAgentContext & context)
{
  context.UnsubscribeAgent<solutionModule::EraseSolutionAgent>();
}

TEST_F(EraseSolutionAgentTest, solutionHasNoElements)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, ERASE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithEmptySolution.scs");

  initialize(context);
  ScAction testActionNode = context.ConvertToAction(context.SearchElementBySystemIdentifier("test_action_node"));
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(testActionNode.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testActionNode.IsFinishedSuccessfully());
  shutdown(context);
}

TEST_F(EraseSolutionAgentTest, solutionHasSomeElements)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, ERASE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithNotEmptySolution.scs");

  initialize(context);

  ScAddr const & variable = context.SearchElementBySystemIdentifier("_variable");
  EXPECT_TRUE(variable.IsValid());
  auto const & classesForVariableIterator =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::VarPermPosArc, variable);
  EXPECT_TRUE(classesForVariableIterator->Next());
  ScAddr const & arcFromClassToVariable = classesForVariableIterator->Get(1);
  EXPECT_FALSE(classesForVariableIterator->Next());
  EXPECT_EQ(context.GetElementEdgesAndOutgoingArcsCount(arcFromClassToVariable), 4u);

  ScAddr const & conceptSolution = context.SearchElementBySystemIdentifier("concept_solution");
  EXPECT_TRUE(conceptSolution.IsValid());
  EXPECT_EQ(utils::IteratorUtils::getAllWithType(&context, conceptSolution, ScType::ConstNode).size(), 2u);

  ScAction testActionNode = context.ConvertToAction(context.SearchElementBySystemIdentifier("test_action_node"));
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(testActionNode.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testActionNode.IsFinishedSuccessfully());
  EXPECT_EQ(context.GetElementEdgesAndOutgoingArcsCount(arcFromClassToVariable), 1u);
  EXPECT_EQ(utils::IteratorUtils::getAllWithType(&context, conceptSolution, ScType::ConstNode).size(), 1u);
  shutdown(context);
}

TEST_F(EraseSolutionAgentTest, solutionIsInvalid)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, ERASE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithoutSolution.scs");

  initialize(context);
  ScAction testActionNode = context.ConvertToAction(context.SearchElementBySystemIdentifier("test_action_node"));
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(testActionNode.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testActionNode.IsFinishedWithError());
  shutdown(context);
}

}  // namespace eraseSolutionAgentTest
