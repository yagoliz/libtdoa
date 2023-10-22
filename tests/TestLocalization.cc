// SPDX-License-Identifier: Apache-2.0

// Copyright 2023 Yago Lizarribar


#include <cmath>

#include <gtest/gtest.h>

#include "../include/Receiver.hh"
#include "../include/TdoaLocator.hh"

TEST(TestLocalization, testLinear) {
    auto r1 = tdoapp::Receiver{0.0, 0.0, 5.0};
    auto r2 = tdoapp::Receiver{3.0, 1.0, 3.0};
    auto r3 = tdoapp::Receiver{0.0, 3.0, std::sqrt(10.0)};

    auto r = std::vector<tdoapp::Receiver> {r1,r2,r3};
    auto result = tdoapp::exactTDOA(r, true);

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

TEST(TestLocalization, testLs) {
    auto r1 = tdoapp::Receiver{0.0, 0.0, 5.0};
    auto r2 = tdoapp::Receiver{3.0, 1.0, 3.0};
    auto r3 = tdoapp::Receiver{0.0, 3.0, std::sqrt(10.0)};
    auto r4 = tdoapp::Receiver{6.0, 4.0, 3.0};

    auto r = std::vector<tdoapp::Receiver> {r1,r2,r3,r4};
    auto result = tdoapp::linearTDOA(r);

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

TEST(TestLocalization, testLsWithPseudo) {
    auto r1 = tdoapp::Receiver{0.0, 0.0, 5.0};
    auto r2 = tdoapp::Receiver{3.0, 1.0, 3.0};
    auto r3 = tdoapp::Receiver{0.0, 3.0, std::sqrt(10.0)};
    auto r4 = tdoapp::Receiver{6.0, 4.0, 3.0};
    auto r5 = tdoapp::Receiver{3.0, 14.0, 10.0};

    auto r = std::vector<tdoapp::Receiver> {r1,r2,r3,r4,r5};
    auto result = tdoapp::linearTDOA(r);

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

TEST(TestLocalization, testNLLS) {
    auto r1 = tdoapp::Receiver{0.0, 0.0, 5.0};
    auto r2 = tdoapp::Receiver{3.0, 1.0, 3.0};
    auto r3 = tdoapp::Receiver{0.0, 3.0, std::sqrt(10.0)};
    auto r4 = tdoapp::Receiver{6.0, 4.0, 3.0};
    auto r5 = tdoapp::Receiver{3.0, 14.0, 10.0};

    auto r = std::vector<tdoapp::Receiver> {r1,r2,r3,r4,r5};
    auto result = tdoapp::nonlinearOptimization(r,Eigen::Vector2d {0.0,0.0});

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

TEST(TestLocalization, testFull) {
    auto r1 = tdoapp::Receiver{0.0, 0.0, 5.0};
    auto r2 = tdoapp::Receiver{3.0, 1.0, 3.0};
    auto r3 = tdoapp::Receiver{0.0, 3.0, std::sqrt(10.0)};
    auto r4 = tdoapp::Receiver{6.0, 4.0, 3.0};
    auto r5 = tdoapp::Receiver{3.0, 14.0, 10.0};

    // Initial solution with linear methods
    auto r = std::vector<tdoapp::Receiver> {r1,r2,r3,r4,r5};
    auto init = tdoapp::initialGuess(r);

    EXPECT_NEAR(init[0],3.0,1e-5);
    EXPECT_NEAR(init[1],4.0,1e-5);

    // Refine with Non-Linear methods
    auto result = tdoapp::nonlinearOptimization(r, init);

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}