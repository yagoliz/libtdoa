// SPDX-License-Identifier: Apache-2.0

// Copyright 2023 Yago Lizarribar


#ifndef LIBDTDOA_RECEIVER_H
#define LIBDTDOA_RECEIVER_H

namespace tdoapp {

    const double kSPEEDOFLIGHT = 299'792'458.0;

    class Receiver {
    public:
        Receiver(const double x, const double y, const double t): x{x}, y{y}, timestamp{t} {}
        Receiver(const double x, const double y): x{x}, y{y}, timestamp{0.0} {}

        double x, y;
        double timestamp;
    };
}

#endif //LIBDTDOA_RECEIVER_H
