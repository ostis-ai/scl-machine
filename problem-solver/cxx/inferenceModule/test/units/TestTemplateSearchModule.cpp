/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-test-framework/sc_test_unit.hpp"
#include "catch2/catch.hpp"
#include "builder/src/scs_loader.hpp"
#include "searcher/TemplateSearcher.hpp"
#include "keynodes/InferenceKeynodes.hpp"

namespace inferenceTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = TEMPLATE_SEARCH_MODULE_TEST_SRC_PATH "/testStructures/";
const std::string SC_MEMORY_INI = "sc-memory.ini";
const std::string TEST_SEARCH_TEMPLATE_ID = "search_template";

void initialize()
{
  inference::InferenceKeynodes::InitGlobal();
}

TEST_CASE("search with content - no structures test case", "[template search manager]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkDynamicArguments");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentNoStructures.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  REQUIRE(searchResults.empty());

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("search with content - empty results test case", "[template search manager]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkDynamicArguments");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentEmptyResultsTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  REQUIRE(searchResults.empty());

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("search with content - single result test case", "[template search manager]")
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkDynamicArguments");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentSingleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  REQUIRE(searchResults.size() == 1);
  REQUIRE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("search with content - multiple result test case", "[template search manager]")
{
  std::string firstCorrectResultLinkIdentifier = "first_correct_result_link";
  std::string secondCorrectResultLinkIdentifier = "second_correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkDynamicArguments");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithContentMultipleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  REQUIRE(searchResults.size() == 2);
  REQUIRE(searchResults[1][searchLinkIdentifier] == context.HelperFindBySystemIdtf(firstCorrectResultLinkIdentifier));
  REQUIRE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(secondCorrectResultLinkIdentifier));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("search without content - no structures test case", "[template search manager]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkDynamicArguments");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithoutContentNoStructures.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;

  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  REQUIRE(searchResults.empty());

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("search without content - single result test case", "[template search manager]")
{
  std::string correctResultLinkIdentifier = "correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkDynamicArguments");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithoutContentSingleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  REQUIRE(searchResults.size() == 1);
  REQUIRE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(correctResultLinkIdentifier));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("search without content - multiple result test case", "[template search manager]")
{
  std::string firstCorrectResultLinkIdentifier = "first_correct_result_link";
  std::string secondCorrectResultLinkIdentifier = "second_correct_result_link";
  std::string searchLinkIdentifier = "search_link";

  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkDynamicArguments");

  loader.loadScsFile(context,TEST_FILES_DIR_PATH + "searchWithoutContentMultipleResultTestStucture.scs");
  initialize();

  ScAddr searchTemplateAddr = context.HelperFindBySystemIdtf(TEST_SEARCH_TEMPLATE_ID);
  inference::TemplateSearcher templateSearcher = inference::TemplateSearcher(&context);
  ScTemplateParams templateParams;
  std::vector<ScTemplateSearchResultItem> searchResults = templateSearcher.searchTemplate(searchTemplateAddr, templateParams);

  REQUIRE(searchResults.size() == 2);
  REQUIRE(searchResults[1][searchLinkIdentifier] == context.HelperFindBySystemIdtf(firstCorrectResultLinkIdentifier));
  REQUIRE(searchResults[0][searchLinkIdentifier] == context.HelperFindBySystemIdtf(secondCorrectResultLinkIdentifier));

  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
}//namespace inferenceTest
