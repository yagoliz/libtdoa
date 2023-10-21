// SPDX-License-Identifier: Apache-2.0

// Copyright 2023 Yago Lizarribar


#ifndef LIBDTDOA_RECEIVER_H
#define LIBDTDOA_RECEIVER_H

namespace libtdoa {

    const double kSPEEDOFLIGHT = 299'792'458.0;

    class Receiver {
    public:
        double x, y;
        double timestamp;
    };
}

#endif //LIBDTDOA_RECEIVER_H
