/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "catch2/catch.hpp"
#include "builder/src/scs_loader.hpp"

#include "manager/DirectInferenceManager.hpp"
#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

namespace directInferenceLogicTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/LogicModule/";
const std::string SC_MEMORY_INI = "sc-memory.ini";
const std::string QUESTION_IDENTIFIER = "inference_logic_test_question";

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

TEST_CASE("complex, true logic rule", "[inference logic test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceLogicTrueComplexRuleTest.scs");
  initialize();

  ScAddr test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  ScAddr targetTemplate = utils::IteratorUtils::getFirstByOutRelation(&context,
      test, scAgentsCommon::CoreKeynodes::rrel_1);
  ScAddr ruleSet = utils::IteratorUtils::getFirstByOutRelation(&context, test,
      scAgentsCommon::CoreKeynodes::rrel_2);
  ScAddr argumentSet = utils::IteratorUtils::getFirstByOutRelation(&context, test,
      scAgentsCommon::CoreKeynodes::rrel_3);

  REQUIRE(targetTemplate.IsValid());
  REQUIRE(ruleSet.IsValid());
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);
  REQUIRE(answer.IsValid());
  REQUIRE(context.HelperCheckEdge(InferenceKeynodes::concept_success_solution,
      answer, ScType::EdgeAccessConstPosPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("complex, false logic rule", "[inference logic test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "generateReplyMessageTest");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "inferenceLogicFalseComplexRuleTest.scs");
  initialize();

  ScAddr test = context.HelperResolveSystemIdtf(QUESTION_IDENTIFIER);

  ScAddr targetTemplate = utils::IteratorUtils::getFirstByOutRelation(&context,
      test, scAgentsCommon::CoreKeynodes::rrel_1);
  ScAddr ruleSet = utils::IteratorUtils::getFirstByOutRelation(&context, test,
      scAgentsCommon::CoreKeynodes::rrel_2);
  ScAddr argumentSet = utils::IteratorUtils::getFirstByOutRelation(&context, test,
      scAgentsCommon::CoreKeynodes::rrel_3);

  REQUIRE(targetTemplate.IsValid());
  REQUIRE(ruleSet.IsValid());
  REQUIRE(argumentSet.IsValid());

  DirectInferenceManager inferenceManager(&context);
  ScAddr answer = inferenceManager.applyInference(targetTemplate, ruleSet, argumentSet);
  REQUIRE(answer.IsValid());
  REQUIRE(!context.HelperCheckEdge(InferenceKeynodes::concept_success_solution, answer, ScType::EdgeAccessConstPosPerm));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

} //namespace directInferenceLogicTest
