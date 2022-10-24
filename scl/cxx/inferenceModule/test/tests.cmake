set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_BIN_PATH}/inference-tests)

include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${SC_MACHINE_ROOT}/tools)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
		NAME inference-module-tests-starter
		DEPENDS sc-agents-common sc-builder-lib inferenceModule
		INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test ${CMAKE_CURRENT_LIST_DIR}/.. ${SC_MACHINE_ROOT}/sc-tools/sc-builder/src)

add_definitions (-DTEMPLATE_SEARCH_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")
