#!/bin/bash

#
# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) 2023 Yago Lizarribar
#

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DBUILD_EXECUTABLES=ON -DBUILD_BENCHMARKS=ON -G Ninja
cmake --build . -j 8
