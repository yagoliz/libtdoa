// SPDX-License-Identifier: Apache-2.0

// Copyright 2023 Yago Lizarribar


#ifndef LIBTDOA_TDOAERROR_HH
#define LIBTDOA_TDOAERROR_HH

#include <ceres/jet.h>
#include "Receiver.hh"
#include "Algebra.hh"

namespace tdoapp {
    class TdoaError {
        const Receiver r1_, r2_;
        static constexpr double epsilon = 1e-8;

    public:
        TdoaError(const Receiver& r1, const Receiver& r2) : r1_(r1), r2_(r2) {}

        template <typename T>
        bool operator()(const T *x, const T* y, T* residual) const {
            T d1 = ceres::sqrt(ceres::pow(r1_.x-x[0],2) + ceres::pow(r1_.y-y[0],2)+epsilon);
            T d2 = ceres::sqrt(ceres::pow(r2_.x-x[0],2) + ceres::pow(r2_.y-y[0],2)+epsilon);

            residual[0] = (r1_.timestamp - r2_.timestamp) - (d1 - d2);
            return true;
        };
    };
}

#endif //LIBTDOA_TDOAERROR_HH
