# TdoaCLI stuff
find_package(Boost REQUIRED program_options)
include_directories(${Boost_INCLUDE_DIRS})

# Benchmark Mean
add_executable(BenchmarkMean BenchmarkMean.cc)
target_link_libraries(BenchmarkMean tdoapp ${Boost_LIBRARIES})
