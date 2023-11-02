# tdoa++ - Time-Difference of Arrival (TDOA) in C++

This is a small library `tdoapp` that solves the TDOA equations in 3 different ways:

- Exact solution. Based on Fang's algorithm, when the number of receivers is 3.
- Least Squares. When the number of sensors > 3
- Non-Linear Least Squares. It uses the [ceres-solver](https://github.com/ceres-solver/ceres-solver)'s primitives to
  solve the nonlinear problem of the TDOA equations.

Note: This work is preliminary and it's missing a bit of functionality.

## Requirements

You'll need a few libraries to compile this software:

- Eigen (version 3.3)
- Ceres-Solver (version 2.0+)
- GTest (Optional, for tests)
- nlohmann (Optional, for TdoaCLI utility)
- Boost (Optional, for TdoaCLI utility)
- [Drogon](https://github.com/drogonframework/drogon) (Optional, for TdoaRest webserver). This requires manual
  installation. Please refer to their official documentation for the steps to build it in your platform.

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
cmake .. -DBUILD_TESTS={ON,OFF(Default)} -DBUILD_EXECUTABLES={ON(Default),OFF}
make -j$nproc
```

If you build with test support, you can run the tests by issuing this command:

```bash
ctest
```

## Running the executables

### TdoaCLI

After building, you will find the executable in `build/src/TdoaCLI` with the following options:

```bash
TdoaCLI. Command-Line utility to solve TDOA problems.
Allowed options::
  -h [ --help ]                 produce help message
  -r [ --receiver ] arg         JSON file with receiver positions & timestamps
  -m [ --method ] arg (=1)      Method to use (1: linear, 2: nonlinear).
                                Default: 1
  -o [ --output ] arg (=stdout) Where to dump the output: (stdout or file)
```

You will need a file with the receivers and timestamps. The format is as specified
in `templates/receiver-template.json`.

### TdoaRest

The TdoaRest interface runs as a webserver (based on the [Drogon framework](https://github.com/drogonframework/drogon))
which by default will listen on `http://localhost:8095/locate`. The whole list of options is:

```bash
TdoaCLI. Command-Line utility to solve TDOA problems.
Allowed options::
  -h [ --help ]                        produce help message
  -p [ --port ] arg (=8095)            Port number
  -i [ --ip-address ] arg (=0.0.0.0)   IP address to listen to
  -e [ --api-endpoint ] arg (=/locate) Where to create the localization API
                                       endpoint
  -l [ --log-path ] arg (=/tmp)        Logging path
  -t [ --thread-num ] arg (=8)         Number of threads for the server
```

To test that it's working, you may use the `curl` command and execute a POST request as follows:

```bash
curl -s -X POST "http://localhost:8095/locate"  -H 'Content-Type: application/json' -d @templates/server-template.json
```

Note that you need to provide a JSON file with the format defined in `templates/server-template.json`.

#### Docker Images

[Docker images](https://hub.docker.com/r/yagoliz/tdoapp) are available for architectures `amd64` and `aarch64`. You can run them as:
```bash
docker run -p 8095:8095 -d yagoliz/tdoapp
```