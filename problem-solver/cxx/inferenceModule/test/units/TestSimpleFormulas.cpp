/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_test.hpp"
#include "scs_loader.hpp"

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"

#include "keynodes/InferenceKeynodes.hpp"

#include "agent/DirectInferenceAgent.hpp"

using namespace inference;

namespace directInferenceSimpleFormulasTest
{
ScsLoader loader;
string const TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/LogicModule/SimpleFormulas/";
string const QUESTION_IDENTIFIER = "inference_logic_test_question";

using InferenceSimpleFormulasTest = ScMemoryTest;
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

// a -> b; Simple test with only one implication that must generates one class to the argument. Four arguments
TEST_F(InferenceSimpleFormulasTest, TrueSimpleLogicRule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSimpleRuleTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("four_arguments_action");
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

// a -> b; Simple test with only one implication that must generates one class to the argument. Three arguments
TEST_F(InferenceSimpleFormulasTest, TrueSimpleLogicRuleThreeArguments)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSimpleRuleTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf("three_arguments_action");
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

// a -> b; b -> c. Should apply both of them to achieve the target
TEST_F(InferenceSimpleFormulasTest, TrueDoubleApplyLogicRule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueDoubleApplyRuleTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only one class of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, action, ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr argumentClassIteratorAfter =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only three classes of argument: one was before agent run and two were generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

// a -> b. Should apply the rule from the second rules set
TEST_F(InferenceSimpleFormulasTest, TrueSecondRulesSet)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSecondRulesSetTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only one class of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, action, ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr argumentClassIteratorAfter =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only two classes of argument: one was before agent run and one was generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

// a -> b. Nothing to generate in two rules set
TEST_F(InferenceSimpleFormulasTest, NothingToGenerateSecondRulesSet)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "nothingToGenerateSecondRulesSetTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only one class of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, action, ScType::EdgeAccessConstPosPerm));

  ScIterator3Ptr argumentClassIteratorAfter =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only one class of argument, nothing was generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

TEST_F(InferenceSimpleFormulasTest, TwoTriplesTest)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "twoTriplesTest.scs");
  initialize();

  ScAddr const & action = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);
  EXPECT_TRUE(action.IsValid());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, action);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, action, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, action, ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

TEST_F(InferenceSimpleFormulasTest, SubstitutionsTest)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "substitutionTestFirst.scs");

  initialize();

  ScAddr const & first_inference_logic_test_question = context.HelperFindBySystemIdtf("first_inference_logic_test_question");
  EXPECT_TRUE(first_inference_logic_test_question.IsValid());
  EXPECT_TRUE(context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, first_inference_logic_test_question).IsValid());
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, first_inference_logic_test_question, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(scAgentsCommon::CoreKeynodes::question_finished_successfully, first_inference_logic_test_question, ScType::EdgeAccessConstPosPerm));

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "substitutionTestSecond.scs");

  ScAddr const & second_inference_logic_test_question = context.HelperFindBySystemIdtf("second_inference_logic_test_question");
  EXPECT_TRUE(second_inference_logic_test_question.IsValid());
  EXPECT_TRUE(context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, second_inference_logic_test_question).IsValid());
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, second_inference_logic_test_question, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(scAgentsCommon::CoreKeynodes::question_finished_successfully, second_inference_logic_test_question, ScType::EdgeAccessConstPosPerm));


  ScAddr const & nrel_idtf = context.HelperFindBySystemIdtf("nrel_idtf");
  EXPECT_TRUE(nrel_idtf.IsValid());
  ScAddr const & second_target_class = context.HelperFindBySystemIdtf("second_target_class");
  EXPECT_TRUE(second_target_class.IsValid());
  ScIterator3Ptr iterator3Ptr = context.Iterator3(second_target_class, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  EXPECT_TRUE(iterator3Ptr->Next());
  ScAddr second_target_class_element = iterator3Ptr->Get(2);
  EXPECT_TRUE(second_target_class_element.IsValid());

  ScTemplate scTemplate;
  scTemplate.Triple(
      ScType::NodeVar >> "_class",
      ScType::EdgeAccessVarPosPerm,
      second_target_class_element
  );
  scTemplate.Triple(
      second_target_class,
      ScType::EdgeAccessVarPosPerm,
      second_target_class_element
  );
  scTemplate.TripleWithRelation(
      "_class",
      ScType::EdgeDCommonVar,
      ScType::LinkVar,
      ScType::EdgeAccessVarPosPerm,
      nrel_idtf
  );
  ScTemplateSearchResult searchResult;
  context.HelperSearchTemplate(scTemplate, searchResult);
  EXPECT_TRUE(searchResult.Size() == 1);
  shutdown();
  context.Destroy();
}

}  // namespace directInferenceSimpleFormulasTest