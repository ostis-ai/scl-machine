project (ostis-inference)
cmake_minimum_required (VERSION 2.8)

set(SC_MACHINE_PATH "${CMAKE_CURRENT_LIST_DIR}/ostis-web-platform/sc-machine")
include("${SC_MACHINE_PATH}/CMakeLists.txt")

# platform doesn't support multi-extensions. Have to put project binaries to sc-machine extensions
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

# for multi-config builds (e.g. msvc)
foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${SC_EXTENSIONS_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${SC_EXTENSIONS_DIRECTORY})
endforeach(OUTPUT_CONFIG CMAKE_CONFIGURATION_TYPES)

add_subdirectory(problem-solver)
