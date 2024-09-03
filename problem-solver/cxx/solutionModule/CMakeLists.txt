file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

list(FILTER SOURCES EXCLUDE REGEX ".*/test/.*")

include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC})

add_library(solutionModule SHARED ${SOURCES})
target_link_libraries(solutionModule sc-memory sc-agents-common)

if (${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()
