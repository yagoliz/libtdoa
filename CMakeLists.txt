# Basic setup
cmake_minimum_required(VERSION 3.26)
project(tdoapp)

set(CMAKE_CXX_STANDARD 17)

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
add_subdirectory(src)

# Testing
enable_testing()
add_subdirectory(tests)
