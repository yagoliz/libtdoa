find_package(GTest REQUIRED)

add_executable(TestAlgebra TestAlgebra.cc)
target_link_libraries(TestAlgebra GTest::GTest GTest::Main Eigen3::Eigen Ceres::ceres)

add_executable(TestTdoaError TestTdoaError.cc)
target_link_libraries(TestTdoaError GTest::GTest GTest::Main Eigen3::Eigen Ceres::ceres tdoapp)

add_executable(TestLocalization TestLocalization.cc)
target_link_libraries(TestLocalization GTest::GTest GTest::Main Eigen3::Eigen Ceres::ceres tdoapp)

# Register the test with CMake's testing system
gtest_add_tests(TARGET TestAlgebra)
gtest_add_tests(TARGET TestTdoaError)
gtest_add_tests(TARGET TestLocalization)