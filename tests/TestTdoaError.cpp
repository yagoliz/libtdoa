// Copyright (c) 2023.

//
// Created by Yago Lizarribar on 21/10/23.
//

#include <gtest/gtest.h>
#include "../include/Receiver.h"
#include "../include/TdoaError.h"

TEST(TestTdoaError, testTdoaError) {
auto r1 = libtdoa::Receiver{1.0, 1.0, 4.0};
auto r2 = libtdoa::Receiver{2.0, 4.0, 8.0};

auto err = libtdoa::TdoaError{r1, r2};

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