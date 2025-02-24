/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <inference/direct_inference_agent.hpp>

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

using namespace inference;

namespace directInferenceComplexFormulasTest
{
ScsLoader loader;
std::string const TEST_FILES_DIR_PATH = "../test-structures/logic-module/complex-formulas/";
std::string const ACTION_IDENTIFIER = "inference_logic_test_action";
std::string const ARGUMENT_IDENTIFIER = "argument";

using InferenceComplexFormulasTest = ScMemoryTest;
int const WAIT_TIME = 1500;

void initialize(ScAgentContext & context)
{
  context.SubscribeAgent<inference::DirectInferenceAgent>();
}

void shutdown(ScAgentContext & context)
{
  context.UnsubscribeAgent<inference::DirectInferenceAgent>();
}

// (a -> b) -> c
TEST_F(InferenceComplexFormulasTest, TrueNestedImplicationFormula)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueNestedImplicationFormula.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier(ARGUMENT_IDENTIFIER);
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only one class of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScIterator3Ptr argumentClassIteratorAfter =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only two classes of argument: one was before agent run and two was generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

// (a || b) -> c
TEST_F(InferenceComplexFormulasTest, TrueDisjunctionImplicationFormula)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "disjunctionImplicationTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier(ARGUMENT_IDENTIFIER);
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only one class of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScIterator3Ptr argumentClassIteratorAfter =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only two classes of argument: one was before agent run and one was generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

// (a && b) -> c
TEST_F(InferenceComplexFormulasTest, TrueConjunctionImplicationFormula)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "conjunctionImplicationTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier(ARGUMENT_IDENTIFIER);
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScIterator3Ptr argumentClassIteratorAfter =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only three classes of argument: one was before agent run and two were generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

// TODO (MksmOrlov): doesn't pass because of empty negation replacements
// (!a) -> b
TEST_F(InferenceComplexFormulasTest, DISABLED_TrueNegationImplicationLogicRule)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueNegationImplicationRuleTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;

  ScAddr argument = context.SearchElementBySystemIdentifier(ARGUMENT_IDENTIFIER);
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScIterator3Ptr argumentClassIteratorAfter =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only three classes of argument: one was before agent run and two were generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

// TODO (MksmOrlov): doesn't pass because of empty negation replacements
// ((a || b) && (!c)) -> d
TEST_F(InferenceComplexFormulasTest, DISABLED_TrueComplexLogicRule)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "inferenceLogicTrueComplexRuleTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;

  ScAddr argument = context.SearchElementBySystemIdentifier(ARGUMENT_IDENTIFIER);
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only two classes of argument before agent run
  EXPECT_TRUE(argumentClassIteratorBefore->Next());
  EXPECT_TRUE(argumentClassIteratorBefore->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorBefore->Next());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScIterator3Ptr argumentClassIteratorAfter =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  // There is only three classes of argument: one was before agent run and two were generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

// ((a || b) && (!c)) -> d
TEST_F(InferenceComplexFormulasTest, FalseLogicRule)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "inferenceLogicFalseComplexRuleTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  shutdown(context);
  context.Destroy();
}

TEST_F(InferenceComplexFormulasTest, EquivalencesNested)
{
  ScAgentContext context;

  std::string const & name = TEST_FILES_DIR_PATH + "inferenceTestEquivalences.scs";

  loader.loadScsFile(context, name);
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedUnsuccessfully());

  shutdown(context);
  context.Destroy();
}

}  // namespace directInferenceComplexFormulasTest
