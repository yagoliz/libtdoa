# Basic setup
cmake_minimum_required(VERSION 3.26)
project(libtdoa)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package (Eigen3 REQUIRED NO_MODULE)
find_package(Ceres REQUIRED)

# Compiling our precious library
add_library(libtdoa SHARED lib/TdoaLocator.cpp
        include/Receiver.h
        include/TdoaError.h
        include/Algebra.h)
target_link_libraries(libtdoa Eigen3::Eigen Ceres::ceres)

# Testing
enable_testing()
add_subdirectory(tests)
