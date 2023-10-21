// Copyright (c) 2023.

//
// Created by Yago Lizarribar on 20/10/23.
//

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
