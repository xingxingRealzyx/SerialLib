cmake_minimum_required(VERSION 3.10)

# Project information
project(SerialLib 
    VERSION 1.0.0
    DESCRIPTION "Linux C++ Serial Communication Library"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compilation options
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(include)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create static library
add_library(serial_static STATIC ${SOURCES} ${HEADERS})
target_include_directories(serial_static PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set library output name
set_target_properties(serial_static PROPERTIES OUTPUT_NAME serial)

# Example programs
add_subdirectory(examples)

# Installation configuration
include(GNUInstallDirs)

# Install library
install(TARGETS serial_static
    EXPORT SerialLibTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install headers
install(FILES
    include/SerialPort.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install CMake configuration files
install(EXPORT SerialLibTargets
    FILE SerialLibTargets.cmake
    NAMESPACE SerialLib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SerialLib
)

# Create and install config file
include(CMakePackageConfigHelpers)

# Generate the config file that includes the exports
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SerialLibConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SerialLibConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SerialLib
)

# Generate the version file for the config file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SerialLibConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install the configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SerialLibConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SerialLibConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SerialLib
)

# Create uninstall target
if(NOT TARGET uninstall)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

# Package information
set(CPACK_PACKAGE_NAME "SerialLib")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Linux C++ Serial Communication Library")
set(CPACK_PACKAGE_VENDOR "SerialLib Project")

include(CPack) 