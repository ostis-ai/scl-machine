/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "keynodes/InferenceKeynodes.hpp"

#include "searcher/templateSearcher/TemplateSearcherGeneral.hpp"
#include "searcher/templateSearcher/TemplateSearcherOnlyAccessEdgesInStructures.hpp"

#include "utils/ReplacementsUtils.hpp"

#include <algorithm>

#include <sc_test.hpp>
#include <scs_loader.hpp>

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

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
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

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
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

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
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
      searchResults.at(context.HelperFindBySystemIdtf(searchLinkIdentifier))[0],
      context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
  EXPECT_EQ(
      searchResults.at(context.HelperFindBySystemIdtf(nodeAlias))[0],
      context.HelperFindBySystemIdtf(firstConstantNode));
}

TEST_F(TemplateSearchManagerTest, SearchInMultipleStructuresWithContent_SingleResultTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";
  std::string nodeAlias = "_node";
  std::string firstConstantNode = "first_constant_node";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentSingleResultTestStructure.scs");

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherInStructures templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  ScAddrUnorderedSet variables;
  ScAddr const & inputStructure1 = context.HelperFindBySystemIdtf("input_structure_1");
  ScAddr const & inputStructure2 = context.HelperFindBySystemIdtf("input_structure_2");
  templateSearcher.setInputStructures({inputStructure1, inputStructure2});
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddrUnorderedSet templateVars = {vars.cbegin(), vars.cend()};
  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(
      searchResults.at(context.HelperFindBySystemIdtf(searchLinkIdentifier))[0],
      context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
  EXPECT_EQ(
      searchResults.at(context.HelperFindBySystemIdtf(nodeAlias))[0],
      context.HelperFindBySystemIdtf(firstConstantNode));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_NoStructuresTestCase)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentNoStructures.scs");

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
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

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
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
      searchResults.at(context.HelperFindBySystemIdtf(searchLinkIdentifier))[0],
      context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
  EXPECT_EQ(
      searchResults.at(context.HelperFindBySystemIdtf(nodeAlias))[0],
      context.HelperFindBySystemIdtf(firstConstantNode));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_EmptyLinkTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentEmptyLinkTest.scs");

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
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
      searchResults.at(context.HelperFindBySystemIdtf(searchLinkIdentifier))[0],
      context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_EmptyLinkTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentEmptyLinkTest.scs");

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
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
      searchResults.at(context.HelperFindBySystemIdtf(searchLinkIdentifier))[0],
      context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithExistedConstructionsTest)
{
  std::string const & structure1Identifier = "test_structure_1";
  std::string const & structure2Identifier = "test_structure_2";
  std::string const & structure3Identifier = "test_structure_3";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithExistedConstructionsStructure.scs");

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  ScAddrVector templateVars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddr const & structure1 = context.HelperFindBySystemIdtf(structure1Identifier);
  ScAddr const & structure2 = context.HelperFindBySystemIdtf(structure2Identifier);
  ScAddr const & structure3 = context.HelperFindBySystemIdtf(structure3Identifier);

  std::unique_ptr<inference::TemplateSearcherAbstract> templateSearcher =
      std::make_unique<inference::TemplateSearcherOnlyAccessEdgesInStructures>(&context);
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

TEST_F(TemplateSearchManagerTest, SearchWithoutAccessEdgesTest)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchStructuresWithoutAccessEdges.scs");

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  ScAddrVector templateVars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);

  std::unique_ptr<inference::TemplateSearcherAbstract> templateSearcher =
      std::make_unique<inference::TemplateSearcherOnlyAccessEdgesInStructures>(&context);
  // input structures are empty because search template does not have access edges
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
