file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

list(FILTER SOURCES EXCLUDE REGEX ".*/test/.*")

set(SOLUTION_MODULE_GENERATED_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)
include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${SOLUTION_MODULE_GENERATED_DIR})

add_library(solutionModule SHARED ${SOURCES})
target_link_libraries(solutionModule sc-memory sc-agents-common)

sc_codegen_ex(solutionModule ${CMAKE_CURRENT_LIST_DIR} ${SOLUTION_MODULE_GENERATED_DIR})

if (${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()
