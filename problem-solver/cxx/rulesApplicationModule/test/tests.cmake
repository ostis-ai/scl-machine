set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_BIN_PATH}/inference-tests)

file(GLOB RULE_APPLICATION_TEST_AGENTS
	"${CMAKE_CURRENT_LIST_DIR}/keynodes/*.cpp"	"${CMAKE_CURRENT_LIST_DIR}/keynodes/*.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/searcher/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/searcher/*.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/agents/*.cpp"	"${CMAKE_CURRENT_LIST_DIR}/agents/*.hpp"
)

add_library(rule_application_test_agents STATIC ${RULE_APPLICATION_TEST_AGENTS})

target_link_libraries(
    rule_application_test_agents
	rules-application
	sc-agents-common)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
		NAME rule_application_test_starter
		DEPENDS rule_application_test_agents sc-builder-lib
		INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test ${SC_COMMON_MODULE_SRC} ${SC_MACHINE_ROOT}/sc-tools/sc-builder/src)

add_definitions(-DRULES_APPLICATION_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")
