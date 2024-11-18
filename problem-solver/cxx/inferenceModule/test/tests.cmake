set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_BIN_PATH}/inference-tests)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
		NAME inference-module-tests-starter
		DEPENDS sc-agents-common sc-builder-lib sc-memory sc-core inferenceModule
		INCLUDES ${SC_MEMORY_SRC} ${SC_BUILDER_PATH} )
add_definitions(-DTEMPLATE_SEARCH_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")
