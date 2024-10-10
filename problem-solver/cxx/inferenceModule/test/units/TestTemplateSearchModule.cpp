/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "searcher/templateSearcher/TemplateSearcherGeneral.hpp"
#include "searcher/templateSearcher/TemplateSearcherOnlyMembershipArcsInStructures.hpp"

#include "utils/ReplacementsUtils.hpp"

#include <algorithm>

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>

namespace inferenceTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/TemplateSearchModule/";
const std::string TEST_SEARCH_TEMPLATE_ID = "search_template";

using TemplateSearchManagerTest = ScMemoryTest;

TEST_F(TemplateSearchManagerTest, SearchWithContent_NoStructuresTestCase)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentNoStructures.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;

  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  EXPECT_TRUE(searchResults.empty());
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_EmptyResultsTestCase)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentEmptyResultsTestStucture.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;

  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  EXPECT_TRUE(searchResults.empty());
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_SingleResultTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";
  std::string nodeAlias = "_node";
  std::string firstConstantNode = "first_constant_node";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentSingleResultTestStructure.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddrUnorderedSet templateVars = {vars.cbegin(), vars.cend()};
  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(
      searchResults.at(context.SearchElementBySystemIdentifier(searchLinkIdentifier))[0],
      context.SearchElementBySystemIdentifier(correctResultLinkIdentifier));
  EXPECT_EQ(
      searchResults.at(context.SearchElementBySystemIdentifier(nodeAlias))[0],
      context.SearchElementBySystemIdentifier(firstConstantNode));
}

TEST_F(TemplateSearchManagerTest, SearchInMultipleStructuresWithContent_SingleResultTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";
  std::string nodeAlias = "_node";
  std::string firstConstantNode = "first_constant_node";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentSingleResultTestStructure.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherInStructures templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  ScAddr const & inputStructure1 = context.SearchElementBySystemIdentifier("input_structure_1");
  ScAddr const & inputStructure2 = context.SearchElementBySystemIdentifier("input_structure_2");
  templateSearcher.setInputStructures({inputStructure1, inputStructure2});
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddrUnorderedSet templateVars = {vars.cbegin(), vars.cend()};
  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(
      searchResults.at(context.SearchElementBySystemIdentifier(searchLinkIdentifier))[0],
      context.SearchElementBySystemIdentifier(correctResultLinkIdentifier));
  EXPECT_EQ(
      searchResults.at(context.SearchElementBySystemIdentifier(nodeAlias))[0],
      context.SearchElementBySystemIdentifier(firstConstantNode));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_NoStructuresTestCase)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentNoStructures.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;

  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  EXPECT_TRUE(searchResults.empty());
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_SingleResultTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";
  std::string nodeAlias = "_node";
  std::string firstConstantNode = "first_constant_node";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentSingleResultTestStucture.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddrUnorderedSet templateVars = {vars.cbegin(), vars.cend()};
  EXPECT_EQ(searchResults.size(), templateVars.size());

  EXPECT_EQ(
      searchResults.at(context.SearchElementBySystemIdentifier(searchLinkIdentifier))[0],
      context.SearchElementBySystemIdentifier(correctResultLinkIdentifier));
  EXPECT_EQ(
      searchResults.at(context.SearchElementBySystemIdentifier(nodeAlias))[0],
      context.SearchElementBySystemIdentifier(firstConstantNode));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_EmptyLinkTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentEmptyLinkTest.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddrUnorderedSet templateVars = {vars.cbegin(), vars.cend()};
  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(
      searchResults.at(context.SearchElementBySystemIdentifier(searchLinkIdentifier))[0],
      context.SearchElementBySystemIdentifier(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_EmptyLinkTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentEmptyLinkTest.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddrUnorderedSet templateVars = {vars.cbegin(), vars.cend()};
  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(
      searchResults.at(context.SearchElementBySystemIdentifier(searchLinkIdentifier))[0],
      context.SearchElementBySystemIdentifier(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithExistedConstructionsTest)
{
  std::string const & structure1Identifier = "test_structure_1";
  std::string const & structure2Identifier = "test_structure_2";
  std::string const & structure3Identifier = "test_structure_3";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithExistedConstructionsStructure.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  ScAddrVector templateVars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddr const & structure1 = context.SearchElementBySystemIdentifier(structure1Identifier);
  ScAddr const & structure2 = context.SearchElementBySystemIdentifier(structure2Identifier);
  ScAddr const & structure3 = context.SearchElementBySystemIdentifier(structure3Identifier);

  std::unique_ptr<inference::TemplateSearcherAbstract> templateSearcher =
      std::make_unique<inference::TemplateSearcherOnlyMembershipArcsInStructures>(&context);
  templateSearcher->setInputStructures({structure1, structure2, structure3});
  templateSearcher->setOutputStructureFillingType(inference::SEARCHED_AND_GENERATED);
  inference::Replacements searchResults;
  templateSearcher->searchTemplate(
      searchTemplateAddr,
      std::vector<ScTemplateParams>{{}},
      {templateVars.cbegin(), templateVars.cend()},
      searchResults);

  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(inference::ReplacementsUtils::getColumnsAmount(searchResults), 1u);
}

TEST_F(TemplateSearchManagerTest, SearchWithoutMembershipArcsTest)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchStructuresWithoutMembershipArcs.scs");

  ScAddr searchTemplateAddr = context.SearchElementBySystemIdentifier(TEST_SEARCH_TEMPLATE_ID);
  ScAddrVector templateVars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);

  std::unique_ptr<inference::TemplateSearcherAbstract> templateSearcher =
      std::make_unique<inference::TemplateSearcherOnlyMembershipArcsInStructures>(&context);
  // input structures are empty because search template does not have membership arcs
  templateSearcher->setInputStructures({});
  templateSearcher->setOutputStructureFillingType(inference::SEARCHED_AND_GENERATED);
  templateSearcher->setReplacementsUsingType(inference::REPLACEMENTS_ALL);
  inference::Replacements searchResults;
  templateSearcher->searchTemplate(
      searchTemplateAddr,
      std::vector<ScTemplateParams>{{}},
      {templateVars.cbegin(), templateVars.cend()},
      searchResults);

  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(inference::ReplacementsUtils::getColumnsAmount(searchResults), 1u);
}
}  // namespace inferenceTest
