file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library (HelloModule SHARED ${SOURCES})

target_include_directories(HelloModule PRIVATE ${SC_MEMORY_SRC})
target_link_libraries(HelloModule sc-memory)

add_dependencies(HelloModule sc-memory)

sc_codegen(HelloModule ${CMAKE_CURRENT_LIST_DIR})
