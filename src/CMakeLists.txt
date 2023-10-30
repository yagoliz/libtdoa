# TdoaCLI stuff
find_package(Boost REQUIRED program_options)
find_package(Drogon CONFIG REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

# TdoaCLI stuff
add_executable(TdoaCLI TdoaCLI.cc)
target_link_libraries(TdoaCLI tdoapp ${Boost_LIBRARIES})

# Setting the RPATH for TdoaCLI
set_target_properties(TdoaCLI PROPERTIES
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
)

# TdoaREest stuff
add_executable(TdoaRest TdoaRest.cc)
target_link_libraries(TdoaRest tdoapp ${Boost_LIBRARIES} Drogon::Drogon)

# Setting the RPATH for TdoaCLI
set_target_properties(TdoaCLI PROPERTIES
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
)

# Setting the RPATH for TdoaCLI
set_target_properties(TdoaRest PROPERTIES
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
)

# Install TdoaCLI if it's built
install(TARGETS TdoaCLI TdoaRest RUNTIME DESTINATION bin)
