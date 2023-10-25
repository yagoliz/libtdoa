# Basic setup
cmake_minimum_required(VERSION 3.26)
project(tdoapp)

set(CMAKE_CXX_STANDARD 17)

# Build options
option(BUILD_EXECUTABLES "Whether to build the executables" ON)
option(BUILD_TESTS "Whether to build to tests" OFF)

# Find required packages
find_package (Eigen3 REQUIRED NO_MODULE)
find_package(Ceres REQUIRED)

# Compiling our precious library
add_library(tdoapp SHARED lib/TdoaLocator.cc
        include/Receiver.hh
        include/TdoaError.hh
        include/Algebra.hh)
target_link_libraries(tdoapp Eigen3::Eigen Ceres::ceres)

# Executables
if (BUILD_EXECUTABLES)
    add_subdirectory(src)
endif (BUILD_EXECUTABLES)

# Testing
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif (BUILD_TESTS)

# Installation
install(TARGETS tdoapp
        EXPORT tdoappTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include/tdoapp
        FILES_MATCHING PATTERN "*.hh")

# Uninstallation target
if(NOT TARGET uninstall)
    configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
            IMMEDIATE @ONLY)

    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

# Cmake export
include(CMakePackageConfigHelpers)

# Create tdoappConfig.cmake
configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/tdoappConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/tdoappConfig.cmake
        INSTALL_DESTINATION lib/cmake/tdoapp
)

# Create tdoappConfigVersion.cmake
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/tdoappConfigVersion.cmake
        VERSION "1.0.0" # specify your library version here
        COMPATIBILITY SameMajorVersion
)

# Install the library's export set (targets)
install(EXPORT tdoappTargets
        FILE tdoappTargets.cmake
        NAMESPACE tdoapp::
        DESTINATION lib/cmake/tdoapp
)

# Install the config and version files
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/tdoappConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/tdoappConfigVersion.cmake
        DESTINATION lib/cmake/tdoapp
)