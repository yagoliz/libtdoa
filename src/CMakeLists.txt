find_package(Boost REQUIRED program_options)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(TdoaCLI TdoaCLI.cc)
target_link_libraries(TdoaCLI tdoapp Eigen3::Eigen Ceres::ceres ${Boost_LIBRARIES})

# Setting the RPATH for TdoaCLI
set_target_properties(TdoaCLI PROPERTIES
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
        BUILD_WITH_INSTALL_RPATH TRUE
)

# Install TdoaCLI if it's built
install(TARGETS TdoaCLI RUNTIME DESTINATION bin)
