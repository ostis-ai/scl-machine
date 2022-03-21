/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc_test.hpp"
#include "builder/src/scs_loader.hpp"

#include "manager/DirectInferenceManager.hpp"
#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

namespace directInferenceLogicTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/LogicModule/";
const std::string QUESTION_IDENTIFIER = "inference_logic_test_question";

using InferenceLogigTest = ScMemoryTest;

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

TEST_F(InferenceLogigTest, TrueLogicRule)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceLogicTrueComplexRuleTest.scs");
  initialize();

  ScAddr test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  ScAddr targetTemplate = utils::IteratorUtils::getAnyByOutRelation(&context,
      test, scAgentsCommon::CoreKeynodes::rrel_1);
  ScAddr ruleSet = utils::IteratorUtils::getAnyByOutRelation(&context, test,
      scAgentsCommon::CoreKeynodes::rrel_2);
  ScAddr argumentSet = utils::IteratorUtils::getAnyByOutRelation(&context, test,
      scAgentsCommon::CoreKeynodes::rrel_3);

  EXPECT_TRUE(targetTemplate.IsValid());
  EXPECT_TRUE(ruleSet.IsValid());
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);
  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution,
      answer, ScType::EdgeAccessConstPosPerm));


}

TEST_F(InferenceLogigTest, FalseLogicRule)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceLogicFalseComplexRuleTest.scs");
  initialize();

  ScAddr test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  ScAddr targetTemplate = utils::IteratorUtils::getAnyByOutRelation(&context,
      test, scAgentsCommon::CoreKeynodes::rrel_1);
  ScAddr ruleSet = utils::IteratorUtils::getAnyByOutRelation(&context, test,
      scAgentsCommon::CoreKeynodes::rrel_2);
  ScAddr argumentSet = utils::IteratorUtils::getAnyByOutRelation(&context, test,
      scAgentsCommon::CoreKeynodes::rrel_3);

  EXPECT_TRUE(targetTemplate.IsValid());
  EXPECT_TRUE(ruleSet.IsValid());
  EXPECT_TRUE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);
  EXPECT_TRUE(answer.IsValid());
  EXPECT_TRUE(!context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));
}

} //namespace directInferenceLogicTest
