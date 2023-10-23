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
