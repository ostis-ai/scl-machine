file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library (HelloService SHARED ${SOURCES})

target_include_directories(HelloService PRIVATE ${SC_MEMORY_SRC})
target_link_libraries(HelloService sc-memory)

add_dependencies(HelloService sc-memory)

sc_codegen(HelloService ${CMAKE_CURRENT_LIST_DIR})
