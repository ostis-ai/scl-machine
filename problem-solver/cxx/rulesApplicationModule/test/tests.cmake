set(TEST_FILES
		"${CMAKE_CURRENT_LIST_DIR}/units/TestRuleApplicationAgent.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/units/TestTemplateSearchModule.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/agents/ConnectionCheckAgent.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/agents/ConnectionCheckAgent.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/keynodes/TestKeynodes.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/keynodes/TestKeynodes.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/searcher/TestResultConstructionSearcher.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/searcher/TestResultConstructionSearcher.hpp")

add_library(rules-application-test ${TEST_FILES})

include_directories(${SC_MEMORY_SRC}/tests/sc-memory/_test ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(rules-application-test sc-builder-lib rules-application gtest)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
		NAME rule_application_test_starter
		DEPENDS sc-builder-lib rules-application-test
		INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test ${SC_COMMON_MODULE_SRC})

target_link_libraries(
		rule_application_test_starter
		sc-agents-common
		rules-application
		sc-builder-lib)

add_definitions(-DRULES_APPLICATION_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")
