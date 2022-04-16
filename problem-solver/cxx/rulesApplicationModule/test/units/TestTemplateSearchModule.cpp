/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <sc_test.hpp>
#include "builder/src/scs_loader.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "templateSearcher/TemplateSearcher.hpp"
#include "keynodes/Keynodes.hpp"

namespace rulesApplicationModule
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = RULES_APPLICATION_MODULE_TEST_SRC_PATH
                                        "/testStructures/ruleConstructionsSearcher/";
const std::string SC_MEMORY_INI = "sc-memory.ini";
const std::string TEST_SEARCH_TEMPLATE_ID = "search_template";
const std::string TEMPLATE_TO_SEARCH_IN = "template_to_search_in";

using TemplateSearcherTest = ScMemoryTest;

void initialize()
{
  rulesApplicationModule::Keynodes::InitGlobal();
  scAgentsCommon::CoreKeynodes::InitGlobal();
}

TEST_F(TemplateSearcherTest, with_content_empty_results)
{
  ScMemoryContext context(sc_access_lvl_make_min, "with_content_empty_results");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentEmptyResultsTestStucture.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.empty());

  context.Destroy();
}

TEST_F(TemplateSearcherTest, with_content_single_result)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext context(sc_access_lvl_make_min, "with_content_single_result");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentSingleResultTestStucture.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));

  context.Destroy();
}

TEST_F(TemplateSearcherTest, with_content_multiple_result)
{
  std::string firstCorrectResultLinkIdentifier = "first_correct_result_link";
  std::string secondCorrectResultLinkIdentifier = "second_correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext context(sc_access_lvl_make_min, "with_content_multiple_result");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentMultipleResultTestStucture.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.size() == 2);
  EXPECT_TRUE(searchResults[1][searchLinkIdentifier] == context.HelperFindBySystemIdtf(firstCorrectResultLinkIdentifier));
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(secondCorrectResultLinkIdentifier));

  context.Destroy();
}

TEST_F(TemplateSearcherTest, without_content_no_structures)
{
  ScMemoryContext context(sc_access_lvl_make_min, "without_content_no_structures");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentNoStructures.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.empty());

  context.Destroy();
}

TEST_F(TemplateSearcherTest, without_content_single_result)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext context(sc_access_lvl_make_min, "without_content_single_result");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentSingleResultTestStucture.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));

  context.Destroy();
}

TEST_F(TemplateSearcherTest, without_content_multiple_result)
{
  std::string firstCorrectResultLinkIdentifier = "first_correct_result_link";
  std::string secondCorrectResultLinkIdentifier = "second_correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext context(sc_access_lvl_make_min, "without_content_multiple_result");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentMultipleResultTestStucture.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.size() == 2);
  EXPECT_TRUE(searchResults[1][searchLinkIdentifier] == context.HelperFindBySystemIdtf(firstCorrectResultLinkIdentifier));
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(secondCorrectResultLinkIdentifier));

  context.Destroy();
}

TEST_F(TemplateSearcherTest, with_content_selective)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext context(sc_access_lvl_make_min, "with_content_selective");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentSelectiveSearchTestStucture.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));

  context.Destroy();
}

TEST_F(TemplateSearcherTest, without_content_empty_link)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext context(sc_access_lvl_make_min, "without_content_empty_link");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithoutContentEmptyLinkTest.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));

  context.Destroy();
}

TEST_F(TemplateSearcherTest, with_content_empty_link)
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  ScMemoryContext context(sc_access_lvl_make_min, "with_content_empty_link");

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "searchWithContentEmptyLinkTest.scs");
  initialize();

  ScAddr templateToSearchIn = context.HelperFindBySystemIdtf(TEMPLATE_TO_SEARCH_IN);
  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  TemplateSearcher templateSearcher = TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplateInStructure(
        searchTemplateAddr,
        templateToSearchIn,
        templateParams);

  EXPECT_TRUE(searchResults.size() == 1);
  EXPECT_TRUE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));

  context.Destroy();
}

}
