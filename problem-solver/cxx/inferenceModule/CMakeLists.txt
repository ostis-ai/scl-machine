set(SC_INFERENCE_MODULE_SRC "${CXX_AGENTS_ROOT}/inferenceModule")

set(SOURCES
		"inferenceModule.cpp"
		"keynodes/InferenceKeynodes.cpp"
		"agent/DirectInferenceAgent.cpp"
		"agent/AReverseInference.cpp"
		"agent/ATemplateGeneration.cpp"
		"manager/DirectInferenceManager.cpp"
		)

set(HEADERS
		"inferenceModule.hpp"
		"keynodes/InferenceKeynodes.hpp"
		"agent/DirectInferenceAgent.hpp"
		"agent/AReverseInference.hpp"
		"agent/ATemplateGeneration.hpp"
		"manager/InferenceManager.hpp"
		"manager/DirectInferenceManager.hpp"
		"model/SolutionTreeNode.hpp"
		)

include_directories(${SC_INFERENCE_MODULE_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library(inferenceModule SHARED ${SOURCES} ${HEADERS})
add_dependencies(inferenceModule sc-memory-cpp sc-kpm-common sc-agents-common)
target_link_libraries(inferenceModule sc-memory-cpp sc-kpm-common sc-agents-common)

sc_codegen(inferenceModule ${SC_INFERENCE_MODULE_SRC})

