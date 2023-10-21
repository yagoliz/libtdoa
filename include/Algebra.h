// SPDX-License-Identifier: Apache-2.0

// Copyright 2023 Yago Lizarribar


#ifndef LIBTDOA_ALGEBRA_H
#define LIBTDOA_ALGEBRA_H

#include <cmath>

namespace libtdoa {

    inline double norm_sq(const double x, const double y) {
        return std::pow(x,2) + std::pow(y,2);
    }

    inline double norm(const double x, const double y) {
        return std::sqrt(std::pow(x,2) + std::pow(y,2));
    }

    int sgn(double val) {
        return (double(0) < val) - (val < double(0));
    }
}

#endif //LIBTDOA_ALGEBRA_H
