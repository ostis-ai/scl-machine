make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
	NAME inference-module-tests
	DEPENDS sc-machine::sc-builder-lib inference-module
	INCLUDES ${INFERENCE_SRC} ${INFERENCE_MODULE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(inference-module-tests)
endif()
