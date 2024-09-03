/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "agent/DeleteSolutionAgent.hpp"

#include <sc_test.hpp>
#include <scs_loader.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>

namespace deleteSolutionAgentTest
{
ScsLoader loader;
const std::string DELETE_SOLUTION_MODULE_TEST_FILES_DIR_PATH =
    SOLUTION_MODULE_TEST_SRC_PATH "/testStructures/deleteSolution/";
const int WAIT_TIME = 5000;

using DeleteSolutionAgentTest = ScMemoryTest;

void initialize(ScAgentContext & context)
{
  context.SubscribeAgent<solutionModule::DeleteSolutionAgent>();
}

void shutdown(ScAgentContext & context)
{
  context.UnsubscribeAgent<solutionModule::DeleteSolutionAgent>();
}

TEST_F(DeleteSolutionAgentTest, solutionHasNoElements)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, DELETE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithEmptySolution.scs");

  initialize(context);
  ScAction testActionNode = context.ConvertToAction(context.HelperFindBySystemIdtf("test_action_node"));
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(testActionNode.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testActionNode.IsFinishedSuccessfully());
  shutdown(context);
}

TEST_F(DeleteSolutionAgentTest, solutionHasSomeElements)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, DELETE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithNotEmptySolution.scs");

  initialize(context);

  ScAddr const & variable = context.HelperFindBySystemIdtf("_variable");
  EXPECT_TRUE(variable.IsValid());
  auto const & classesForVariableIterator =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessVarPosPerm, variable);
  EXPECT_TRUE(classesForVariableIterator->Next());
  ScAddr const & edgeFromClassToVariable = classesForVariableIterator->Get(1);
  EXPECT_FALSE(classesForVariableIterator->Next());
  EXPECT_EQ(context.GetElementOutputArcsCount(edgeFromClassToVariable), 4u);

  ScAddr const & conceptSolution = context.HelperFindBySystemIdtf("concept_solution");
  EXPECT_TRUE(conceptSolution.IsValid());
  EXPECT_EQ(utils::IteratorUtils::getAllWithType(&context, conceptSolution, ScType::NodeConst).size(), 2u);

  ScAction testActionNode = context.ConvertToAction(context.HelperFindBySystemIdtf("test_action_node"));
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(testActionNode.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testActionNode.IsFinishedSuccessfully());
  EXPECT_EQ(context.GetElementOutputArcsCount(edgeFromClassToVariable), 1u);
  EXPECT_EQ(utils::IteratorUtils::getAllWithType(&context, conceptSolution, ScType::NodeConst).size(), 1u);
  shutdown(context);
}

TEST_F(DeleteSolutionAgentTest, solutionIsInvalid)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, DELETE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithoutSolution.scs");

  initialize(context);
  ScAction testActionNode = context.ConvertToAction(context.HelperFindBySystemIdtf("test_action_node"));
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(testActionNode.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testActionNode.IsFinishedUnsuccessfully());
  shutdown(context);
}

}  // namespace deleteSolutionAgentTest
