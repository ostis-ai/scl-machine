set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_BIN_PATH}/inference-tests)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
		NAME solution-module-tests-starter
		DEPENDS sc-agents-common sc-builder-lib solutionModule
		INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test ${SC_MACHINE_ROOT}/sc-tools/sc-builder/src ${CMAKE_CURRENT_LIST_DIR}/..)
add_definitions(-DSOLUTION_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")
