// SPDX-License-Identifier: Apache-2.0

// Copyright 2023 Yago Lizarribar


#ifndef LIBDTDOA_TDOALOCATOR_H
#define LIBDTDOA_TDOALOCATOR_H

#include <vector>

#include <Eigen/Dense>
#include <ceres/ceres.h>

#include "Receiver.h"
#include "TdoaError.h"

namespace libtdoa{
    // Linearized TDOA equations
    Eigen::Vector2d initialGuess(const std::vector<Receiver>& receivers);
    Eigen::Vector2d linearTDOA(const std::vector<Receiver>& receivers);
    Eigen::Vector2d exactTDOA(const std::vector<Receiver>& receivers, bool getPositive = true);

    // Non-linear optimization for TDOA equations
    Eigen::Vector2d nonlinearOptimization(const std::vector<Receiver>& receivers, const Eigen::Vector2d& initialGuess);
}

#endif //LIBDTDOA_TDOALOCATOR_H
