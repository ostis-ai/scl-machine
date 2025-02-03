file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.hpp" "*.cpp"
    "*/*.hpp" "*/*.cpp"
    "*/*/*.hpp" "*/*/*.cpp"
)

list(FILTER SOURCES EXCLUDE REGEX ".*/test/.*")

add_library(inference-module SHARED ${SOURCES})
target_link_libraries(inference-module
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
)
target_include_directories(inference-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(inference-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(inference-module)
endif()


if(${SC_BUILD_TESTS})
    set(INFERENCE_MODULE_SRC ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()
