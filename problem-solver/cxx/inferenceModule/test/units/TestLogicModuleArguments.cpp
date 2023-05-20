/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_test.hpp"
#include "scs_loader.hpp"

#include <sc-memory/kpm/sc_agent.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "keynodes/InferenceKeynodes.hpp"

#include "agent/DirectInferenceAgent.hpp"

using namespace inference;

namespace directInferenceLogicArgumentsTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/LogicModule/";

using InferenceLogicTest = ScMemoryTest;
const int WAIT_TIME = 1500;

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();

  ScAgentInit(true);
  SC_AGENT_REGISTER(inference::DirectInferenceAgent);
}

void shutdown()
{
  SC_AGENT_UNREGISTER(inference::DirectInferenceAgent);
}

// All arguments are valid
// a -> b
TEST_F(InferenceLogicTest, AllArgumentsValid)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("valid_arguments_action");
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, action, ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr argumentClassIteratorAfter =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only 3 classes of argument: two was before agent run and one is generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

// Input structure is invalid
TEST_F(InferenceLogicTest, InvalidArguments)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("invalid_arguments_action");
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully, action, ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

// Input structure is an empty set
TEST_F(InferenceLogicTest, EmptyInputStructure)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("empty_input_structure_action");
  EXPECT_TRUE(action.IsValid());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, action, ScType::EdgeAccessConstPosPerm));
  ScAddr answer =
      utils::IteratorUtils::getAnyByOutRelation(&context, action, scAgentsCommon::CoreKeynodes::nrel_answer);
  ScAddr solution = utils::IteratorUtils::getAnyFromSet(&context, answer);
  EXPECT_TRUE(
      context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, solution, ScType::EdgeAccessConstNegPerm));

  shutdown();
  context.Destroy();
}

// Rule set is an empty set, target is not achieved
TEST_F(InferenceLogicTest, EmptyRuleSet)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("empty_rules_set_queue_action");
  EXPECT_TRUE(action.IsValid());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully, action, ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

// Rule set queue is an empty set, target is not achieved
TEST_F(InferenceLogicTest, EmptyRuleSetQueue)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("empty_rules_set_action");
  EXPECT_TRUE(action.IsValid());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, action, ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

}  // namespace directInferenceLogicArgumentsTest
