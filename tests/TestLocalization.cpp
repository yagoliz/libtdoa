// Copyright (c) 2023.

//
// Created by Yago Lizarribar on 21/10/23.
//

// Copyright (c) 2023.

//
// Created by Yago Lizarribar on 21/10/23.
//

#include <cmath>

#include <gtest/gtest.h>

#include "../include/Receiver.h"
#include "../include/TdoaLocator.h"

TEST(TestLocalization, testLinear) {
    auto r1 = libtdoa::Receiver{0.0, 0.0, 5.0};
    auto r2 = libtdoa::Receiver{3.0, 1.0, 3.0};
    auto r3 = libtdoa::Receiver{0.0, 3.0, std::sqrt(10.0)};

    auto r = std::vector<libtdoa::Receiver> {r1,r2,r3};
    auto result = libtdoa::exactTDOA(r, true);

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

TEST(TestLocalization, testLs) {
    auto r1 = libtdoa::Receiver{0.0, 0.0, 5.0};
    auto r2 = libtdoa::Receiver{3.0, 1.0, 3.0};
    auto r3 = libtdoa::Receiver{0.0, 3.0, std::sqrt(10.0)};
    auto r4 = libtdoa::Receiver{6.0, 4.0, 3.0};

    auto r = std::vector<libtdoa::Receiver> {r1,r2,r3,r4};
    auto result = libtdoa::linearTDOA(r);

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

TEST(TestLocalization, testLsWithPseudo) {
    auto r1 = libtdoa::Receiver{0.0, 0.0, 5.0};
    auto r2 = libtdoa::Receiver{3.0, 1.0, 3.0};
    auto r3 = libtdoa::Receiver{0.0, 3.0, std::sqrt(10.0)};
    auto r4 = libtdoa::Receiver{6.0, 4.0, 3.0};
    auto r5 = libtdoa::Receiver{3.0, 14.0, 10.0};

    auto r = std::vector<libtdoa::Receiver> {r1,r2,r3,r4,r5};
    auto result = libtdoa::linearTDOA(r);

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

TEST(TestLocalization, testNLLS) {
    auto r1 = libtdoa::Receiver{0.0, 0.0, 5.0};
    auto r2 = libtdoa::Receiver{3.0, 1.0, 3.0};
    auto r3 = libtdoa::Receiver{0.0, 3.0, std::sqrt(10.0)};
    auto r4 = libtdoa::Receiver{6.0, 4.0, 3.0};
    auto r5 = libtdoa::Receiver{3.0, 14.0, 10.0};

    auto r = std::vector<libtdoa::Receiver> {r1,r2,r3,r4,r5};
    auto result = libtdoa::nonlinearOptimization(r,Eigen::Vector2d {0.0,0.0});

    EXPECT_NEAR(result[0],3.0,1e-5);
    EXPECT_NEAR(result[1],4.0,1e-5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}