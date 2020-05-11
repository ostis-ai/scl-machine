set(SC_EXAMPLE_MODULE_SRC "${PROBLEM_SOLVER_ROOT}/inferenceModule")

set(SOURCES
		"inferenceModule.cpp"
		"keynodes/keynodes.cpp"
		)

set(HEADERS
		"inferenceModule.hpp"
		"keynodes/keynodes.hpp"
		)

include_directories(${SC_EXAMPLE_MODULE_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library(inferenceModule SHARED ${SOURCES} ${HEADERS})
add_dependencies(inferenceModule sc-memory-cpp sc-kpm-common sc-agents-common)
target_link_libraries(inferenceModule sc-memory-cpp sc-kpm-common sc-agents-common)

sc_codegen(inferenceModule ${SC_EXAMPLE_MODULE_SRC})

