# libtdoa - Time-Difference of Arrival (TDOA) in C++

This is a small library `libtdoa` that solves the TDOA equations in 3 different ways:

- Exact solution. Based on Fang's algorithm, when the number of receivers is 3.
- Least Squares. When the number of sensors > 3
- Non-Linear Least Squares. It uses the [ceres-solver](https://github.com/ceres-solver/ceres-solver)'s primitives to solve the nonlinear problem of the TDOA equations.

Note: This work is preliminary and it's missing a bit of functionality.

## Requirements

You'll need a few libraries to compile this software:

- Eigen
- Ceres-Solver
- GTest (for tests)

To install them you can use the following commands.

### Linux

On Debian-based systems:
```bash
sudo apt install libeigen3-dev libceres-dev libgtest-dev libboost-dev nlohmann-json3-dev
```

On Arch Linux and derivatives:
```bash
sudo pacman -Syyu eigen ceres-solver gtest boost nlohmann-json
```

### MacOS

If you have homebrew installed:
```bash
brew install eigen ceres-solver googletest boost nlohmann-json
```

## Building
Inside the root directory, you can build the package by running:
```bash
mkdir build && cd build
cmake ..
make -j$nproc
```

Afterwards, you can run the tests by issuing this command:
```bash
ctest
```