// SPDX-License-Identifier: Apache-2.0

// Copyright 2023 Yago Lizarribar


#include <gtest/gtest.h>
#include "../include/Receiver.hh"
#include "../include/TdoaError.hh"

TEST(TestTdoaError, testTdoaError) {
auto r1 = tdoapp::Receiver{1.0, 1.0, 4.0};
auto r2 = tdoapp::Receiver{2.0, 4.0, 8.0};

auto err = tdoapp::TdoaError{r1, r2};

double residual = 0.0;
double x = 0.0;
double y = 0.0;

err(&x, &y, &residual);
EXPECT_NEAR(residual, -0.9420776074, 1e-5); // 1e-5 is the allowed error
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}