find_package(Boost REQUIRED program_options)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(TdoaCLI TdoaCLI.cc)
target_link_libraries(TdoaCLI libtdoa Eigen3::Eigen Ceres::ceres ${Boost_LIBRARIES})