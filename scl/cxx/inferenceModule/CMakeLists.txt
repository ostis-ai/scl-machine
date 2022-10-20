set(SOURCES
        "InferenceModule.cpp"
        "keynodes/InferenceKeynodes.cpp"
        "agent/DirectInferenceAgent.cpp"
        "agent/AReverseInference.cpp"
        "agent/ATemplateGeneration.cpp"
        "manager/DirectInferenceManager.cpp"
        "manager/TemplateManager.cpp"
        "generator/SolutionTreeGenerator.cpp"
        "utils/ContainersUtils.cpp"
        "utils/LogicRuleUtils.cpp"
        "comparator/AddrComparator.cpp"
        "searcher/TemplateSearcher.cpp"
        "logic/LogicExpression.cpp"
        )

set(HEADERS
        "InferenceModule.hpp"
        "keynodes/InferenceKeynodes.hpp"
        "agent/DirectInferenceAgent.hpp"
        "agent/AReverseInference.hpp"
        "agent/ATemplateGeneration.hpp"
        "manager/DirectInferenceManager.hpp"
        "manager/TemplateManager.hpp"
        "generator/SolutionTreeGenerator.hpp"
        "utils/ContainersUtils.hpp"
        "utils/LogicRuleUtils.hpp"
        "comparator/AddrComparator.hpp"
        "searcher/TemplateSearcher.hpp"
        "logic/LogicExpression.hpp"
        )

set(INFERENCE_MODULE_GENERATED_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)
include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${INFERENCE_MODULE_GENERATED_DIR})

add_library(inferenceModule SHARED ${SOURCES} ${HEADERS})
target_link_libraries(inferenceModule sc-memory sc-agents-common)

sc_codegen_ex(inferenceModule ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

if (${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()

