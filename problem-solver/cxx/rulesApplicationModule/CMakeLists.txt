set(SOURCES
		"RulesApplicationModule.cpp"
		"keynodes/Keynodes.cpp"
		"agent/RulesApplicationAgent.cpp"
		"manager/RuleApplicationManager.cpp"
		"searcher/RuleConstructionsSearcher.cpp"
		"templateSearcher/TemplateSearcher.cpp"
		"model/StatementsCheckResult.cpp"
		"comparator/ComparatorByNesting.cpp"
		"utils/RuleCheckResultUtils.cpp"
		)

set(HEADERS
		"RulesApplicationModule.hpp"
		"keynodes/Keynodes.hpp"
		"agent/RulesApplicationAgent.hpp"
		"manager/RuleApplicationManager.hpp"
		"searcher/RuleConstructionsSearcher.hpp"
		"templateSearcher/TemplateSearcher.hpp"
		"model/StatementsCheckResult.hpp"
		"comparator/ComparatorByNesting.hpp"
		"utils/RuleCheckResultUtils.hpp"
		)

include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${SC_TOOLS_SRC} ${SC_COMMON_MODULE_SRC})

add_library(rules-application SHARED ${SOURCES} ${HEADERS})
target_link_libraries(rules-application sc-memory sc-agents-common common boost_locale)

sc_codegen_ex(rules-application ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

if (${SC_BUILD_TESTS})
	include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()
