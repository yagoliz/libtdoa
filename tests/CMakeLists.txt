find_package(GTest REQUIRED)

add_executable(TestAlgebra TestAlgebra.cpp)
target_link_libraries(TestAlgebra GTest::GTest GTest::Main Eigen3::Eigen Ceres::ceres)

add_executable(TestTdoaError TestTdoaError.cpp)
target_link_libraries(TestTdoaError GTest::GTest GTest::Main Eigen3::Eigen Ceres::ceres libtdoa)

add_executable(TestLocalization TestLocalization.cpp)
target_link_libraries(TestLocalization GTest::GTest GTest::Main Eigen3::Eigen Ceres::ceres libtdoa)

# Register the test with CMake's testing system
gtest_add_tests(TARGET TestAlgebra)
gtest_add_tests(TARGET TestTdoaError)
gtest_add_tests(TARGET TestLocalization)