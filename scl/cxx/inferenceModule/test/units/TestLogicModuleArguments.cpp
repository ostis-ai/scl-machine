/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "agent/DirectInferenceAgent.hpp"
#include <sc-agents-common/utils/AgentUtils.hpp>
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "sc_test.hpp"
#include "scs_loader.hpp"

#include "manager/DirectInferenceManager.hpp"
#include "keynodes/InferenceKeynodes.hpp"
#include "classifier/FormulaClassifier.hpp"
#include "utils/ReplacementsUtils.hpp"

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
TEST_F(InferenceLogicTest, TrueSimpleLogicRule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("valid_arguments_action");
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore = context.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::action_direct_inference,
        action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        action,
        ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr argumentClassIteratorAfter = context.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        argument);

  // There is only 3 classes of argument: two was before agent run and one is generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

// Input structure is invalid, knowledge_base_IMS is used by default
TEST_F(InferenceLogicTest, InvalidInputStructure)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("invalid_input_structure_action");
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore = context.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::action_direct_inference,
        action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        action,
        ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr argumentClassIteratorAfter = context.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        argument);

  // There is only 3 classes of argument: two was before agent run and one is generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

// Input structure is an empty set, knowledge_base_IMS SHOULD be used by default
// TODO (MksmOrlov): implement checking input structure that it is not empty set, use knowledge_base_IMS
TEST_F(InferenceLogicTest, EmptyInputStructure)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("empty_input_structure_action");
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore = context.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::action_direct_inference,
        action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        action,
        ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr argumentClassIteratorAfter = context.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        argument);

  // There is only 3 classes of argument: two was before agent run and one is generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

// Rule set is an empty set, target is not achieved
TEST_F(InferenceLogicTest, EmptyRuleSet)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("empty_rules_set_queue_action");
  EXPECT_TRUE(action.IsValid());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::action_direct_inference,
        action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully,
        action,
        ScType::EdgeAccessConstPosPerm));

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore = context.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        argument);

  // There is only two classes of argument after agent run, nothing added
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  shutdown();
  context.Destroy();
}

// Rule set queue is an empty set, target is not achieved
// TODO (MksmOrlov): check if rules queue is empty, add debug
TEST_F(InferenceLogicTest, EmptyRuleSetQueue)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("empty_rules_set_action");
  EXPECT_TRUE(action.IsValid());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::action_direct_inference,
        action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully,
        action,
        ScType::EdgeAccessConstPosPerm));

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore = context.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        argument);

  // There is only two classes of argument after agent run, nothing added
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  shutdown();
  context.Destroy();
}

}  // namespace directInferenceLogicArgumentsTest
