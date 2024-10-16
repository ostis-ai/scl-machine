/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "classifier/FormulaClassifier.hpp"

#include "keynodes/InferenceKeynodes.hpp"

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

using namespace inference;

namespace formulaClassifierTest
{
ScsLoader loader;
std::string const TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/FormulaClassifierModule/";

using FormulaClassifierTest = ScMemoryTest;

TEST_F(FormulaClassifierTest, RuleIsImplication)
{
  ScMemoryContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "inferenceLogicTrueComplexRuleTest.scs");

  ScAddr testRule = context.ResolveElementSystemIdentifier("inference_logic_test_rule");
  ScIterator5Ptr formulaIterator = context.CreateIterator5(
      testRule,
      ScType::ConstPermPosArc,
      ScType::Unknown,
      ScType::ConstPermPosArc,
      ScKeynodes::rrel_main_key_sc_element);
  EXPECT_TRUE(formulaIterator->Next());

  ScAddr formula = formulaIterator->Get(2);
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, formula), FormulaClassifier::IMPLICATION_ARC);
  auto const & [begin, end] = context.GetConnectorIncidentElements(formula);
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, begin), FormulaClassifier::CONJUNCTION);
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, end), FormulaClassifier::ATOMIC);

  context.Destroy();
}

TEST_F(FormulaClassifierTest, AtomicLogicalFormulaWithouCLass)
{
  ScMemoryContext context;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "atomicLogicalFormulaTestWithoutClass.scs");

  ScAddr formula = context.ResolveElementSystemIdentifier("formula");
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, formula), FormulaClassifier::ATOMIC);

  ScAddr formulaWithLink = context.ResolveElementSystemIdentifier("formula_with_link");
  EXPECT_EQ(FormulaClassifier::typeOfFormula(&context, formulaWithLink), FormulaClassifier::ATOMIC);

  context.Destroy();
}

}  // namespace formulaClassifierTest
