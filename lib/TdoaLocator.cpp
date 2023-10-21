
#include <Eigen/SVD>

#include "../include/TdoaLocator.h"
#include "../include/Algebra.h"

namespace libtdoa {
    Eigen::Vector2d initialGuess(const std::vector<Receiver> &receivers) {

        // For Least Squares, we need at least 4 receivers (in 2D case)
        Eigen::Vector2d position;
        if (receivers.size() > 3) {
            position = linearTDOA(receivers);
        } else {
            position = exactTDOA(receivers);
        }

        return position;
    }

    Eigen::Vector2d exactTDOA(const std::vector<Receiver> &receivers, bool getPositive) {

        Eigen::Vector2d res{0.0, 0.0};

        // Get receivers in Eigen matrix form
        Eigen::Vector2d s0{receivers[0].x, receivers[0].y};
        Eigen::Vector2d s1{receivers[1].x, receivers[1].y};
        Eigen::Vector2d s2{receivers[2].x, receivers[2].y};

        // Define the rotation matrix
        double theta = std::atan2(receivers[1].y - receivers[0].y, receivers[1].x - receivers[0].x);
        Eigen::Matrix2d R;
        R << std::cos(theta), -std::sin(theta),
             std::sin(theta),  std::cos(theta);

        Eigen::Vector2d s0r = {0.0, 0.0};
        Eigen::Vector2d s1r = R.transpose() * (s1 - s0);
        Eigen::Vector2d s2r = R.transpose() * (s2 - s0);

        // We extract the values for the equations
        double b = s1r[0];
        double cx = s2r[0];
        double cy = s2r[1];
        double c = norm(cx, cy);

        double tau_01 = receivers[0].timestamp - receivers[1].timestamp;
        double tau_02 = receivers[0].timestamp - receivers[2].timestamp;

        // We extract the values for g and h
        double g = ((tau_02/tau_01) * b - cx) / cy;
        double h = (std::pow(c,2) - std::pow(tau_02,2) + tau_01 * tau_02 * (1 - std::pow(b/tau_01,2))) /
                (2 * cy);

        // With this we go for the terms of the quadratic equation
        double d = -(1 + std::pow(g,2) - std::pow(b/tau_01,2));
        double e = b * (1 - std::pow(b/tau_01,2)) - 2 * g *h;
        double f = std::pow(tau_01,2) / 4 * std::pow((1 - std::pow(b/tau_01,2)),2) - std::pow(h,2);

        // Terms for x and y (positions)
        double discriminant = std::pow(e,2) - 4 * d * f;

        double xp, yp, xm, ym;
        if (discriminant >= 0.0) {
            xp = (-e + std::sqrt(discriminant)) / (2 * d);
            yp = g * xp + h;

            xm = (-e - std::sqrt(discriminant)) / (2 * d);
            ym = g * xm + h;
        } else {
            throw std::runtime_error("No real solution exists for the position");
        }

        // Conversion to absolute coordinates
        // For the positive result
        Eigen::Vector2d rp = R * Eigen::Vector2d{xp,yp};
        rp += s0;
        auto rp0 = rp - s0;
        auto rp1 = rp - s1;
        auto rpn = norm(rp0[0], rp0[1]) - norm(rp1[0], rp1[1]);

        // We need to compare whether the signs are the same for the obtained result and the observed tdoa
        bool multiple = false; // Check if we will have multiple solutions
        if (sgn(rpn) == sgn(tau_01)) {
            res[0] = rp[0]; res[1] = rp[1];
            multiple = true;
        }

        // For the negative result
        Eigen::Vector2d rm = R * Eigen::Vector2d{xm,ym};
        rm += s0;

        auto rm0 = rm - s0;
        auto rm1 = rm - s1;
        auto rmn = norm(rm0[0], rm0[1]) - norm(rm1[0], rm1[1]);

        // We need to compare whether the signs are the same for the obtained result and the observed tdoa
        if (sgn(rmn) == sgn(tau_01)) {
            if (multiple) {
                std::cerr << "Warning multiple solutions exist!" << std::endl;
                if (getPositive) {
                    std::cerr << "Positive arm will be returned" << std::endl;
                    return res;
                } else {
                    std::cerr << "Negative arm will be returned" << std::endl;
                }
            }

            res[0] = rm[0]; res[1] = rm[1];
        }

        return res;
    }

    Eigen::Vector2d linearTDOA(const std::vector<Receiver>& receivers)  {
        // Set up for the LS problem
        Eigen::MatrixXd A(receivers.size()-1,3);
        Eigen::VectorXd b(receivers.size()-1);

        for (int i = 0; i < receivers.size()-1; i++) {
            A(i, 0) = -(receivers[0].timestamp - receivers[i+1].timestamp);
            A(i, 1) = receivers[0].x - receivers[i+1].x;
            A(i, 2) = receivers[0].y - receivers[i+1].y;

            b[i] = 0.5 * (
                    std::pow(receivers[0].timestamp - receivers[i+1].timestamp,2)
                    + norm_sq(receivers[0].x, receivers[0].y)
                    - norm_sq(receivers[i+1].x, receivers[i+1].y)
            );
        }

        // We use Eigen's SVD decomposition to solve the problem
        Eigen::Vector3d r;
        if (receivers.size() == 4) {
            r = A.colPivHouseholderQr().solve(b);
        } else {
            r = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
        }
        return Eigen::Vector2d {r[1], r[2]};
    }

    Eigen::Vector2d nonlinearOptimization(const std::vector<Receiver> &receivers,
                                                          const Eigen::Vector2d &initialGuess) {
        ceres::Problem problem;
        auto x = initialGuess[0];
        auto y = initialGuess[1];
        for (size_t i = 0; i < receivers.size() - 1; i++) {
            for (size_t j = i + 1; j < receivers.size(); j++) {
                problem.AddResidualBlock(
                        new ceres::AutoDiffCostFunction<TdoaError, 1, 1, 1>(
                                new TdoaError(receivers[i], receivers[j])
                        ),
                        nullptr,
                        &x, &y
                );
            }
        }

        ceres::Solver::Options options;
        ceres::Solver::Summary summary;
        ceres::Solve(options, &problem, &summary);

        return Eigen::Vector2d{x,y};
    }
}
