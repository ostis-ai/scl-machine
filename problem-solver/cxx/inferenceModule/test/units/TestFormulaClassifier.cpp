/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_test.hpp"
#include "scs_loader.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/InferenceKeynodes.hpp"

#include "classifier/FormulaClassifier.hpp"

using namespace inference;

namespace formulaClassifierTest
{
ScsLoader loader;
std::string const TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/FormulaClassifierModule/";

using FormulaClassifierTest = ScMemoryTest;

void initialize()
{
  InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

TEST_F(FormulaClassifierTest, RuleIsImplication)
{
  ScMemoryContext context(sc_access_lvl_make_min, "implication_detected");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "inferenceLogicTrueComplexRuleTest.scs");
  initialize();

  ScAddr testRule = context.HelperResolveSystemIdtf("inference_logic_test_rule");
  ScIterator5Ptr formulaIterator = context.Iterator5(
      testRule,
      ScType::EdgeAccessConstPosPerm,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_main_key_sc_element);
  EXPECT_TRUE(formulaIterator->Next());

  ScAddr formula = formulaIterator->Get(2);
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, formula), FormulaClassifier::IMPLICATION_EDGE);
  ScAddr begin;
  ScAddr end;
  context.GetEdgeInfo(formula, begin, end);
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, begin), FormulaClassifier::CONJUNCTION);
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, end), FormulaClassifier::ATOMIC);

  context.Destroy();
}

TEST_F(FormulaClassifierTest, AtomicLogicalFormulaWithouCLass)
{
  ScMemoryContext context(sc_access_lvl_make_min);

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "atomicLogicalFormulaTestWithoutClass.scs");
  initialize();

  ScAddr formula = context.HelperResolveSystemIdtf("formula");
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, formula), FormulaClassifier::ATOMIC);

  ScAddr formulaWithLink = context.HelperResolveSystemIdtf("formula_with_link");
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, formulaWithLink), FormulaClassifier::ATOMIC);

  context.Destroy();
}

}  // namespace formulaClassifierTest
