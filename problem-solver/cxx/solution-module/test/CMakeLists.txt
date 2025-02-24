make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
	NAME solution-module-tests
	DEPENDS sc-machine::sc-builder-lib solution-module
	INCLUDES ${SOLUTION_MODULE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(solution-module-tests)
endif()
