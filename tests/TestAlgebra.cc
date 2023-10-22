// SPDX-License-Identifier: Apache-2.0

// Copyright 2023 Yago Lizarribar

#include <gtest/gtest.h>

#include "../include/Algebra.hh"

TEST(TestAlgebra, normSqTest) {
    double x0 = 1.0;
    double y0 = 1.0;

    double normsq = tdoapp::norm_sq(x0, y0);

    EXPECT_NEAR(normsq, 2.0, 1e-5); // 1e-5 is the allowed error
}

TEST(TestAlgebra, norm) {
    double x0 = 1.0;
    double y0 = 1.0;

    double normsq = tdoapp::norm(x0, y0);

    EXPECT_NEAR(normsq, 1.4142135624, 1e-5); // 1e-5 is the allowed error
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}