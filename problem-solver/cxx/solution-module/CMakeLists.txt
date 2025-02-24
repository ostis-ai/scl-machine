file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.hpp" "*.cpp"
    "*/*.hpp" "*/*.cpp"
)

list(FILTER SOURCES EXCLUDE REGEX ".*/test/.*")

add_library(solution-module SHARED ${SOURCES})
target_link_libraries(solution-module
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
)
target_include_directories(solution-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(solution-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(solution-module)
endif()


if(${SC_BUILD_TESTS})
    set(SOLUTION_MODULE_SRC ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()
