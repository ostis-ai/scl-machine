/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_test.hpp"
#include "scs_loader.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "searcher/TemplateSearcher.hpp"
#include "keynodes/InferenceKeynodes.hpp"

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
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentNoStructures.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.empty());
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_EmptyResultsTestCase)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentEmptyResultsTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.empty());
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_SingleResultTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentSingleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_MultipleResultTestCase)
{
  std::string firstCorrectResultLinkIdentifier = "first_correct_result_link";
  std::string secondCorrectResultLinkIdentifier = "second_correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentMultipleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.size() == 2);
  EXPECT_TRUE(searchResults[1][searchLinkIdentifier] == context.HelperFindBySystemIdtf(firstCorrectResultLinkIdentifier));
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(secondCorrectResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_NoStructuresTestCase)
{
  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithoutContentNoStructures.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.empty());
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_SingleResultTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithoutContentSingleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_MultipleResultTestCase)
{
  std::string firstCorrectResultLinkIdentifier = "first_correct_result_link";
  std::string secondCorrectResultLinkIdentifier = "second_correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithoutContentMultipleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.size() == 2);
  EXPECT_TRUE(searchResults[1][searchLinkIdentifier] == context.HelperFindBySystemIdtf(firstCorrectResultLinkIdentifier));
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(secondCorrectResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_SelectiveTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentSelectiveSearchTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithoutContent_EmptyLinkTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithoutContentEmptyLinkTest.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
}

TEST_F(TemplateSearchManagerTest, SearchWithContent_EmptyLinkTestCase)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext& context = *m_ctx;

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentEmptyLinkTest.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));
}
}//namespace inferenceTest
