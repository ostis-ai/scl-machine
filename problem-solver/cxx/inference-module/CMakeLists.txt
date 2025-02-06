file(GLOB SOURCES CONFIGURE_DEPENDS
    "lib/include/inference/*.hpp"
    "lib/src/*.hpp" "lib/src/*.cpp"
    "lib/src/*/*.hpp" "lib/src/*/*.cpp"
    "lib/src/*/*/*.hpp" "lib/src/*/*/*.cpp"
)

add_library(inference SHARED ${SOURCES})
target_link_libraries(inference
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
)
target_include_directories(inference
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/src
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
    PUBLIC $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

file(GLOB SOURCES CONFIGURE_DEPENDS
    "module/*.hpp" "module/*.cpp"
    "module/*/*.hpp" "module/*/*.cpp"
    "module/*/*/*.hpp" "module/*/*/*.cpp"
)

add_library(inference-module SHARED ${SOURCES})
target_link_libraries(inference-module
    LINK_PUBLIC inference
)
target_include_directories(inference-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/module
)
set_target_properties(inference-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(inference-module)
endif()

if(${SC_BUILD_TESTS})
    set(INFERENCE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/lib/src)
    set(INFERENCE_MODULE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/module)
    add_subdirectory(test)
endif()
