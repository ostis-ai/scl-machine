/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_test.hpp"
#include "scs_loader.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "searcher/templateSearcher/TemplateSearcherGeneral.hpp"
#include "searcher/templateSearcher/TemplateSearcherOnlyAccessEdgesInStructures.hpp"
#include "keynodes/InferenceKeynodes.hpp"
#include "utils/ReplacementsUtils.hpp"

#include <algorithm>

namespace inferenceTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/TemplateSearchModule/";
const std::string TEST_SEARCH_TEMPLATE_ID = "search_template";

using TemplateSearchManagerTest = ScMemoryTest;

void initialize()
{
  inference::InferenceKeynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_NoStructuresTestCase)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentNoStructures.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;

  inference::Replacements searchResults;
  inference::ScAddrHashSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  EXPECT_TRUE(searchResults.empty());
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_EmptyResultsTestCase)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentEmptyResultsTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;

  inference::Replacements searchResults;
  inference::ScAddrHashSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  EXPECT_TRUE(searchResults.empty());
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_SingleResultTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";
  std::string node_alias = "_node";
  std::string first_constant_node = "first_constant_node";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentSingleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  inference::ScAddrHashSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  inference::ScAddrHashSet templateVars = {vars.cbegin(), vars.cend()};
  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(
      searchResults.at(context.HelperFindBySystemIdtf(searchLinkIdentifier))[0],
      context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
  EXPECT_EQ(
      searchResults.at(context.HelperFindBySystemIdtf(node_alias))[0],
      context.HelperFindBySystemIdtf(first_constant_node));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_NoStructuresTestCase)
{
  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentNoStructures.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;

  inference::Replacements searchResults;
  inference::ScAddrHashSet variables;
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
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  inference::ScAddrHashSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  inference::ScAddrHashSet templateVars = {vars.cbegin(), vars.cend()};
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
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  inference::ScAddrHashSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  inference::ScAddrHashSet templateVars = {vars.cbegin(), vars.cend()};
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
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcherGeneral templateSearcher(&context);
  ScTemplateParams templateParams;
  inference::Replacements searchResults;
  inference::ScAddrHashSet variables;
  templateSearcher.getVariables(searchTemplateAddr, variables);
  templateSearcher.searchTemplate(searchTemplateAddr, templateParams, variables, searchResults);

  ScAddrVector const & vars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  inference::ScAddrHashSet templateVars = {vars.cbegin(), vars.cend()};
  EXPECT_EQ(searchResults.size(), templateVars.size());
  EXPECT_EQ(
      searchResults.at(context.HelperFindBySystemIdtf(searchLinkIdentifier))[0],
      context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithExistedConstructionsTest)
{
  std::string structure1 = "test_structure_1";
  std::string structure2 = "test_structure_2";
  std::string structure3 = "test_structure_3";

  ScMemoryContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithExistedConstructionsStructure.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  ScAddrVector templateVars = utils::IteratorUtils::getAllWithType(&context, searchTemplateAddr, ScType::Var);
  ScAddr structure_1 = context.HelperFindBySystemIdtf(structure1);
  ScAddr structure_2 = context.HelperFindBySystemIdtf(structure2);
  ScAddr structure_3 = context.HelperFindBySystemIdtf(structure3);

  std::unique_ptr<inference::TemplateSearcherAbstract> templateSearcher =
      std::make_unique<inference::TemplateSearcherOnlyAccessEdgesInStructures>(
        &context);
  templateSearcher->setInputStructures({structure_1, structure_2, structure_3});
  templateSearcher->setOutputStructureFillingType(SEARCHED_AND_GENERATED);
  inference::Replacements searchResults;
  templateSearcher->searchTemplate(
      searchTemplateAddr,
      std::vector<ScTemplateParams>{{}},
      {templateVars.cbegin(), templateVars.cend()},
      searchResults);

  EXPECT_EQ(searchResults.size(), templateVars.size());
}
}  // namespace inferenceTest
