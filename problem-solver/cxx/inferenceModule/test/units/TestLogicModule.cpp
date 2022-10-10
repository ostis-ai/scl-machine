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

namespace directInferenceLogicTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/LogicModule/";
const std::string QUESTION_IDENTIFIER = "inference_logic_test_question";

using InferenceLogicTest = ScMemoryTest;
const int WAIT_TIME = 3000;

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

TEST_F(InferenceLogicTest, TrueLogicRule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "successful_inference");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceLogicTrueComplexRuleTest.scs");
  initialize();

  ScAddr test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::action_direct_inference,
        test);

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, test, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_successfully,
        test,
        ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

TEST_F(InferenceLogicTest, FalseLogicRule)
{
  ScMemoryContext context(sc_access_lvl_make_min, "unsuccessful_inference");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceLogicFalseComplexRuleTest.scs");
  initialize();

  ScAddr test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::action_direct_inference,
        test);
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, test, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully,
        test,
        ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}

TEST_F(InferenceLogicTest, EquivalencesNested)
{
  ScMemoryContext context(sc_access_lvl_make_min, "testSeveralNestedEquivalences");

  auto const & name = TEST_FILES_DIR_PATH + "inferenceTestEquivalences.scs";
  struct stat buffer;
  SC_LOG_DEBUG("exists = " + to_string(stat (name.c_str(), &buffer) == 0));

  loader.loadScsFile(context, name);
  initialize();

  ScAddr test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);


  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        InferenceKeynodes::action_direct_inference,
        test);
  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, test, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
        scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully,
        test,
        ScType::EdgeAccessConstPosPerm));

  shutdown();
  context.Destroy();
}


} //namespace directInferenceLogicTest
