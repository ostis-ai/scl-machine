install(TARGETS 
    inference-object inference
    EXPORT scl-machineExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS
    inference-module solution-module
    EXPORT privateExport
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/extensions"
)

export(EXPORT scl-machineExport 
    NAMESPACE scl-machine::  # to simulate a different name and see it works
    FILE "${CMAKE_CURRENT_BINARY_DIR}/scl-machineTargets.cmake"
)

install(EXPORT scl-machineExport
    FILE scl-machineTargets.cmake
    NAMESPACE scl-machine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scl-machine
)

install(EXPORT privateExport
    FILE privateTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scl-machine
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/scl-machine-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${SCL_MACHINE_ROOT}/cmake/scl-machine-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/scl-machine-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scl-machine
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/scl-machine-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/scl-machine-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scl-machine
)

set(CPACK_PACKAGE_NAME                  scl-machine)
set(CPACK_PACKAGE_VENDOR                "OSTIS AI")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "Software implementation of logic inference machine")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR         ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${PROJECT_VERSION_PATCH})
set(CPACK_VERBATIM_VARIABLES            TRUE)

if (${WIN32})
    set(CPACK_GENERATOR     ZIP)
else()
    set(CPACK_GENERATOR     TGZ)
endif()
include(CPack)
