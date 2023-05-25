file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

list(FILTER SOURCES EXCLUDE REGEX ".*/test/.*")

set(INFERENCE_MODULE_GENERATED_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)
include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${INFERENCE_MODULE_GENERATED_DIR})

add_library(inferenceModule SHARED ${SOURCES})
target_link_libraries(inferenceModule sc-memory sc-agents-common)

sc_codegen_ex(inferenceModule ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

if (${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()
