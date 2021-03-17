set(SOURCES
        "inferenceModule.cpp"
        "keynodes/InferenceKeynodes.cpp"
        "agent/DirectInferenceAgent.cpp"
        "agent/AReverseInference.cpp"
        "agent/ATemplateGeneration.cpp"
        "manager/DirectInferenceManager.cpp"
        "manager/TemplateManager.cpp"
        "generator/SolutionTreeGenerator.cpp"
        "utils/ContainersUtils.cpp"
        "comparator/AddrComparator.cpp"
        )

set(HEADERS
        "inferenceModule.hpp"
        "keynodes/InferenceKeynodes.hpp"
        "agent/DirectInferenceAgent.hpp"
        "agent/AReverseInference.hpp"
        "agent/ATemplateGeneration.hpp"
        "manager/DirectInferenceManager.hpp"
        "manager/TemplateManager.hpp"
        "generator/SolutionTreeGenerator.hpp"
        "utils/ContainersUtils.hpp"
        "comparator/AddrComparator.hpp"
        )

include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC})

add_library(inferenceModule SHARED ${SOURCES} ${HEADERS})
target_link_libraries(inferenceModule sc-memory sc-agents-common)

sc_codegen(inferenceModule ${CMAKE_CURRENT_LIST_DIR})
