/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc_test.hpp"
#include "scs_loader.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"

#include "keynodes/SolutionKeynodes.hpp"

#include "agent/DeleteSolutionAgent.hpp"

namespace deleteSolutionAgentTest
{
ScsLoader loader;
const std::string DELETE_SOLUTION_MODULE_TEST_FILES_DIR_PATH = SOLUTION_MODULE_TEST_SRC_PATH "/testStructures/deleteSolution/";
const int WAIT_TIME = 5000;

using DeleteSolutionAgentTest = ScMemoryTest;

void initialize()
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  solutionModule::SolutionKeynodes::InitGlobal();

  ScAgentInit(true);
  SC_AGENT_REGISTER(solutionModule::DeleteSolutionAgent)
}

void shutdown()
{
  SC_AGENT_UNREGISTER(solutionModule::DeleteSolutionAgent)
}

TEST_F(DeleteSolutionAgentTest, solutionHasNoElements)
{
  ScMemoryContext & context = *m_ctx;
  loader.loadScsFile(context, DELETE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithEmptySolution.scs");

  initialize();
  ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, testActionNode, ScType::EdgeAccessConstPosPerm));
  shutdown();
}

TEST_F(DeleteSolutionAgentTest, solutionHasSomeElements)
{
  ScMemoryContext & context = *m_ctx;
  loader.loadScsFile(context, DELETE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithNotEmptySolution.scs");

  initialize();

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

  ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, testActionNode, ScType::EdgeAccessConstPosPerm));
  EXPECT_EQ(context.GetElementOutputArcsCount(edgeFromClassToVariable), 1u);
  EXPECT_EQ(utils::IteratorUtils::getAllWithType(&context, conceptSolution, ScType::NodeConst).size(), 1u);
  shutdown();
}

TEST_F(DeleteSolutionAgentTest, solutionIsInvalid)
{
  ScMemoryContext & context = *m_ctx;
  loader.loadScsFile(context, DELETE_SOLUTION_MODULE_TEST_FILES_DIR_PATH + "actionWithoutSolution.scs");

  initialize();
  ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully, testActionNode, ScType::EdgeAccessConstPosPerm));
  shutdown();
}

}  // namespace deleteSolutionAgentTest
