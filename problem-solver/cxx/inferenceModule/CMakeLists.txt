file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

list(FILTER SOURCES EXCLUDE REGEX ".*/test/.*")

add_library(inferenceModule SHARED ${SOURCES})
target_link_libraries(inferenceModule
        LINK_PRIVATE sc-memory
        LINK_PRIVATE sc-agents-common)
target_include_directories(inferenceModule PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC})

if (${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()
