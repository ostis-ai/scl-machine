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

namespace directInferenceComplexFormulasTest
{
ScsLoader loader;
string const TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/LogicModule/ComplexFormulas/";
string const QUESTION_IDENTIFIER = "inference_logic_test_question";
string const ARGUMENT_IDENTIFIER = "argument";

using InferenceComplexFormulasTest = ScMemoryTest;
int const WAIT_TIME = 1500;

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

// (a -> b) -> c
TEST_F(InferenceComplexFormulasTest, TrueNestedImplicationFormula)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueNestedImplicationFormula.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf(ARGUMENT_IDENTIFIER);
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

  // There is only two classes of argument: one was before agent run and two was generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

// (a || b) -> c
TEST_F(InferenceComplexFormulasTest, TrueDisjunctionImplicationFormula)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "disjunctionImplicationTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf(ARGUMENT_IDENTIFIER);
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

// (a && b) -> c
TEST_F(InferenceComplexFormulasTest, TrueConjunctionImplicationFormula)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "conjunctionImplicationTest.scs");
  initialize();

  ScAddr action = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.HelperFindBySystemIdtf(ARGUMENT_IDENTIFIER);
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

  // There is only three classes of argument: one was before agent run and two were generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown();
  context.Destroy();
}

// TODO (MksmOrlov): doesn't pass because of empty negation replacements
// (!a) -> b
TEST_F(InferenceComplexFormulasTest, DISABLED_TrueNegationImplicationLogicRule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueNegationImplicationRuleTest.scs");
  initialize();

  ScAddr const test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  ScAddr argument = context.HelperFindBySystemIdtf(ARGUMENT_IDENTIFIER);
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, test);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, test, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, test, ScType::EdgeAccessConstPosPerm));

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

// TODO (MksmOrlov): doesn't pass because of empty negation replacements
// ((a || b) && (!c)) -> d
TEST_F(InferenceComplexFormulasTest, DISABLED_TrueComplexLogicRule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "inferenceLogicTrueComplexRuleTest.scs");
  initialize();

  ScAddr test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  ScAddr argument = context.HelperFindBySystemIdtf(ARGUMENT_IDENTIFIER);
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, test);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, test, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, test, ScType::EdgeAccessConstPosPerm));

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

// ((a || b) && (!c)) -> d
TEST_F(InferenceComplexFormulasTest, FalseLogicRule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "unsuccessful_inference");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "inferenceLogicFalseComplexRuleTest.scs");
  initialize();

  ScAddr const test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, test);
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, test, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully, test, ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

TEST_F(InferenceComplexFormulasTest, EquivalencesNested)
{
  ScMemoryContext context(sc_access_lvl_make_min, "testSeveralNestedEquivalences");

  string const & name = TEST_FILES_DIR_PATH + "inferenceTestEquivalences.scs";
  struct stat buffer;
  SC_LOG_DEBUG("exists = " + to_string(stat(name.c_str(), &buffer) == 0));

  loader.loadScsFile(context, name);
  initialize();

  ScAddr const test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, InferenceKeynodes::action_direct_inference, test);
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, test, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully, test, ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

}  // namespace directInferenceComplexFormulasTest
