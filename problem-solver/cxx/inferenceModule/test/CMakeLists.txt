file(GLOB TEST_FILES "units/*.cpp")
add_executable(inference-module-tests-starter "InferenceModuleTestsStarter.cpp" "${TEST_FILES}")

include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${SC_MACHINE_ROOT}/tools)

target_link_libraries(
        inference-module-tests-starter
        sc-agents-common
        sc-test-framework
        sc-builder-lib
        inferenceModule
        )

add_definitions (-DTEMPLATE_SEARCH_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")
