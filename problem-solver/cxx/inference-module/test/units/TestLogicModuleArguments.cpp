/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "agent/DirectInferenceAgent.hpp"

#include "inference/inference_keynodes.hpp"

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>

using namespace inference;

namespace directInferenceLogicArgumentsTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = "../test-structures/logic-module/";

using InferenceLogicTest = ScMemoryTest;
const int WAIT_TIME = 1500;

void initialize(ScAgentContext & context)
{
  context.SubscribeAgent<inference::DirectInferenceAgent>();
}

void shutdown(ScAgentContext & context)
{
  context.UnsubscribeAgent<inference::DirectInferenceAgent>();
}

// All arguments are valid
// a -> b
TEST_F(InferenceLogicTest, AllArgumentsValid)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier("valid_arguments_action");
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

// Input structure is invalid
TEST_F(InferenceLogicTest, InvalidArguments)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier("invalid_arguments_action");
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  ScAddr argument = context.SearchElementBySystemIdentifier("argument");
  EXPECT_TRUE(argument.IsValid());

  ScIterator3Ptr argumentClassIteratorBefore =
      context.CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, argument);

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedUnsuccessfully());

  shutdown(context);
  context.Destroy();
}

// Input structure is an empty set
TEST_F(InferenceLogicTest, EmptyInputStructure)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier("empty_input_structure_action");
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());
  ScStructure result = action.GetResult();
  ScAddr solution = utils::IteratorUtils::getAnyFromSet(&context, result);
  EXPECT_TRUE(solution.IsValid());
  EXPECT_TRUE(context.CheckConnector(InferenceKeynodes::concept_success_solution, solution, ScType::ConstPermNegArc));

  shutdown(context);
  context.Destroy();
}

// Rule set is an empty set, target is not achieved
TEST_F(InferenceLogicTest, EmptyRuleSet)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier("empty_rules_set_queue_action");
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedUnsuccessfully());

  shutdown(context);
  context.Destroy();
}

// Rule set queue is an empty set, target is not achieved
TEST_F(InferenceLogicTest, EmptyRuleSetQueue)
{
  ScAgentContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "logicModuleArgumentsTest.scs");
  initialize(context);

  ScAddr const & tempActionNode = context.SearchElementBySystemIdentifier("empty_rules_set_action");
  EXPECT_TRUE(tempActionNode.IsValid());
  ScAction action = context.ConvertToAction(tempActionNode);
  ;
  EXPECT_TRUE(action.IsValid());

  EXPECT_TRUE(action.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  shutdown(context);
  context.Destroy();
}

}  // namespace directInferenceLogicArgumentsTest
