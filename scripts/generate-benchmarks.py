#  SPDX-License-Identifier: Apache-2.0
#
#  Copyright (c) 2023 Yago Lizarribar

# std functions
import argparse
import json

# imported modules
import numpy as np


def main():

    # Argument parsing shenanigans
    parser = argparse.ArgumentParser(
        prog='generate-benchmark.py',
        description='Generate benchmark files to test tdoapp capa∆bilities',
        epilog='Project: yagoliz/tdoapp')

    parser.add_argument('-f', '--filename', required=True)  # JSON output filename
    parser.add_argument('-n', '--number-receivers', required=True, type=int)  # Number of receivers to use
    parser.add_argument('-v', '--number-experiments', default=100, type=int)  # How many experiments to generate
    parser.add_argument('-s', '--sigma', default=1.0, type=float)  #

    args = parser.parse_args()

    # Getting the receivers
    r = args.number_receivers
    receivers = 20*np.random.rand(r, 2) - 10

    # Getting the experiments
    n = args.number_experiments
    sigma = args.sigma
    center = np.mean(receivers, axis=0)
    distances = np.linalg.norm(center - receivers, axis=1)
    measurements = distances + sigma*np.random.randn(n, r)

    # Saving dictionary
    benchmark = {'center': center.tolist()}

    # First we get the receivers
    receiver_dict = {}
    for i in range(r):
        receiver_dict[f'{i}'] = receivers[i, :].tolist()
    benchmark['receivers'] = receiver_dict

    # Now we go for the toa values
    measurements_array = []
    for i in range(n):
        current_dict = {}
        for j in range(r):
            current_dict[f'{j}'] = measurements[i, j]

        measurements_array.append(current_dict)

    benchmark['measurements'] = measurements_array

    # convert dict to JSON
    filename = args.filename
    with open(filename, 'w') as f:
        json.dump(benchmark, f)


if __name__ == "__main__":
    main()
