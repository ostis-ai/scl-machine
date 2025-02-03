/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "agent/DirectInferenceAgent.hpp"

#include "keynodes/InferenceKeynodes.hpp"

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

using namespace inference;

namespace directInferenceSimpleFormulasTest
{
ScsLoader loader;
std::string const TEST_FILES_DIR_PATH = "/test-structures/logic-module/simple-formulas/";
std::string const ACTION_IDENTIFIER = "inference_logic_test_action";

using InferenceSimpleFormulasTest = ScMemoryTest;
const int WAIT_TIME = 1500;

void initialize(ScAgentContext & context)
{
  context.SubscribeAgent<inference::DirectInferenceAgent>();
}

void shutdown(ScAgentContext & context)
{
  context.UnsubscribeAgent<inference::DirectInferenceAgent>();
}

// a -> b; Simple test with only one implication that must generates one class to the argument. Four arguments
TEST_F(InferenceSimpleFormulasTest, TrueSimpleLogicRule)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSimpleRuleTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier("four_arguments_action");
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier("argument");
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

  // There is only 3 classes of argument: two was before agent run and one is generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

// a -> b; Simple test with only one implication that must generates one class to the argument. Three arguments
TEST_F(InferenceSimpleFormulasTest, TrueSimpleLogicRuleThreeArguments)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSimpleRuleTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier("three_arguments_action");
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier("argument");
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

  // There is only 3 classes of argument: two was before agent run and one is generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

// a -> b; b -> c. Should apply both of them to achieve the target
TEST_F(InferenceSimpleFormulasTest, TrueDoubleApplyLogicRule)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueDoubleApplyRuleTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier("argument");
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

  // There is only three classes of argument: one was before agent run and two were generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

// a -> b. Should apply the rule from the second rules set
TEST_F(InferenceSimpleFormulasTest, TrueSecondRulesSet)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "trueSecondRulesSetTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier("argument");
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

// a -> b. Nothing to generate in two rules set
TEST_F(InferenceSimpleFormulasTest, NothingToGenerateSecondRulesSet)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "nothingToGenerateSecondRulesSetTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier("argument");
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

  // There is only one class of argument, nothing was generated
  EXPECT_TRUE(argumentClassIteratorAfter->Next());

  // And there is no more classes
  EXPECT_FALSE(argumentClassIteratorAfter->Next());

  shutdown(context);
  context.Destroy();
}

TEST_F(InferenceSimpleFormulasTest, TwoTriplesTest)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "twoTriplesTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  shutdown(context);
  context.Destroy();
}

TEST_F(InferenceSimpleFormulasTest, ApplyRuleFromSecondAndThenFromFirstSetTest)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "applyRuleFromSecondAndThenFromFirstSetTest.scs");

  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier(ACTION_IDENTIFIER);
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());
  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  shutdown(context);
  context.Destroy();
}

// Fails because of empty solution tree
TEST_F(InferenceSimpleFormulasTest, DISABLED_SolutionTreePreventsDoubleRuleApplyingOnSameReplacementsTest)
{
  ScAgentContext context;
  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "solutionTreePreventsDoubleRuleApplyingTest.scs");
  initialize(context);

  ScAction firstInferenceLogicTestAction =
      context.ConvertToAction(context.SearchElementBySystemIdentifier("first_" + ACTION_IDENTIFIER));
  EXPECT_TRUE(firstInferenceLogicTestAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(firstInferenceLogicTestAction.IsFinishedSuccessfully());

  ScAction secondInferenceLogicTestAction =
      context.ConvertToAction(context.SearchElementBySystemIdentifier("second_" + ACTION_IDENTIFIER));
  EXPECT_TRUE(secondInferenceLogicTestAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(secondInferenceLogicTestAction.IsFinishedSuccessfully());

  ScIterator3Ptr iterator = context.CreateIterator3(
      context.SearchElementBySystemIdentifier("class_2"), ScType::ConstPermPosArc, ScType::ConstNode);

  EXPECT_TRUE(iterator->Next());
  EXPECT_FALSE(iterator->Next());

  ScAddr const & treeTemplate = context.SearchElementBySystemIdentifier("solution_tree_template");
  ScTemplate scTemplate;
  context.BuildTemplate(scTemplate, treeTemplate);
  ScTemplateSearchResult result;
  context.SearchByTemplate(scTemplate, result);
  EXPECT_EQ(result.Size(), 1u);
  shutdown(context);
  context.Destroy();
}

}  // namespace directInferenceSimpleFormulasTest
