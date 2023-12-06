// SPDX-License-Identifier: Apache-2.0
//
// Copyright (c) 2023 Yago Lizarribar

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <boost/program_options.hpp>
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

#include "../include/Receiver.hh"
#include "../include/TdoaLocator.hh"

using std::cout;
using std::cerr;
using std::endl;

namespace po = boost::program_options;

using nlohmann::json;

struct options {
    int optimization_level = 1;
    int window_size = 1;
    std::string receiver_file;
    std::string output;
};

struct benchmarkResult {
    long long time;
    double x;
    double y;

    benchmarkResult(long long time, double x, double y) : time{time}, x{x}, y{y} {}
};

int parse_commandline(int argc, char **argv, options &opt) {

    po::options_description desc("TdoaCLI. Command-Line utility to solve TDOA problems.\nAllowed options:");
    desc.add_options()
            ("help,h", "produce help message")
            ("receiver,r", po::value<std::string>(), "JSON file with receiver positions & timestamps")
            ("method,m", po::value<int>(&opt.optimization_level)->default_value(1),
             "Method to use (1: linear, 2: nonlinear). Default: 1")
            ("window-size,w", po::value<int>(&opt.window_size)->default_value(1),
             "Size of the averaging window. Default: 1")
            ("output,o", po::value<std::string>(&opt.output)->default_value("stdout"),
             "Where to dump the output: (stdout or file)");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // Help text
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    // Receiver file is mandatory
    if (vm.count("receiver")) {
        auto receiver_file = vm["receiver"].as<std::string>();
        cout << "Selected receiver file: " << receiver_file << endl;
        opt.receiver_file = receiver_file;
    } else {
        cerr << "Must provide receiver positions" << endl;
        cerr << desc << endl;
        return 1;
    }

    // Get the window size
    if (vm.count("window-size")) {
        auto window_size = vm["window-size"].as<int>();
        cout << "Averaging window size was set to: " << window_size << "." << endl;
        opt.window_size = window_size;
    }
    else {
        cout << "Window size was not set. Window size of 10 will be used" << endl;
    }

    // Last we select the method
    if (vm.count("method")) {
        int method = vm["method"].as<int>();
        std::string m = method == 1 ? "Linear/Least Squares" : "Non-Linear Least Squares";
        cout << "Optimization Method was set to: " << m << "." << endl;
        opt.optimization_level = method;
    } else {
        cout << "Method level was not set. Linear/Least Squares optimization will be run" << endl;
        cout << desc << endl;
    }

    return 0;
}

int main(int argc, char **argv) {
    // Command line options
    auto opt = std::make_unique<options>();
    if (parse_commandline(argc, argv, *opt)) {
        return 1;
    }

    // Get receivers information
    std::ifstream ifs(opt->receiver_file);
    if (!ifs.is_open()) {
        cerr << "Error: Could not open receiver file" << endl;
        return 1;
    }

    // Receiver file should contain a vector with a "receivers" & "measurements" field
    auto receivers = json::parse(ifs);

    // For this benchmark, we assume static receivers
    std::vector<tdoapp::Receiver> receiver_array;
    if (!receivers.contains("receivers")) {
        cerr << "Error parsing receiver file. Could not find receivers field" << endl;
        return 1;
    }

    for (const auto &[key, receiver]: receivers["receivers"].items()) {
        if (receiver.is_array() && receiver.size() == 2) {
            receiver_array.emplace_back(receiver[0].get<double>(), receiver[1].get<double>());
        } else {
            cerr << "Wrong format for JSON value in receivers. Expected 2-element array for receiver" << endl
                 << "The array assertion was: " << receiver.is_array() << ". The size was: " << receiver.size()
                 << "" << endl;
        }
    }

    // We get the number of receivers for later verification
    auto R = receiver_array.size();
    auto W = opt->window_size;

    // We get now all measurement members
    unsigned  long N; // Number of total measurements
    if (receivers.contains("measurements")) {
        N = receivers["measurements"].size();
        if (N < W) {
            cerr << "Not enough measurements: " << N << " for the selected window size "
                 << W << endl;
            return 1;
        }
    } else {
        cerr << "Error parsing receiver file. Could not find TOA values" << endl;
        return 1;
    }

    // Read the first all measurements and insert them in matrix
    auto toas = Eigen::MatrixXd(N,R);
    int row = 0;
    for (const auto &measurement: receivers["measurements"]) {
        // Verify that the number of TOA values is the same as receivers
        if (measurement.size() != R) {
            cerr << "Incorrect number of measurements need " << R << " as it is the number of receivers" << endl;
            return 1;
        }

        // Insert elements in the resulting matrix
        int col = 0;
        for (const auto &[key, values]: measurement.items()) {
            toas(row,col) = values.get<double>();
            col += 1;
        }

        row += 1;
    }

    // Benchmarking init
    std::vector<benchmarkResult> result;
    result.reserve(N-W+1);
    for (int i = 0; i < N - W; i++) {
        // Start our timer
        const auto start{std::chrono::high_resolution_clock::now()};

        // Core localization approach with window sizes
        auto slice = toas(Eigen::seq(i,i+W-1),Eigen::all);
        auto toas_mean = slice.colwise().mean();

        // We collect the timestamps in the receiver array
        for (int j = 0; j < R; j++){
            receiver_array[j].timestamp = toas_mean[j];
        }

        // Position computation
        auto estimation = tdoapp::initialGuess(receiver_array);
        if (opt->optimization_level == 2) {
            estimation = tdoapp::nonlinearOptimization(receiver_array, estimation);
        }
        // End timer and collect
        const auto end{std::chrono::high_resolution_clock::now()};
        auto t = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        result.emplace_back(t, estimation[0], estimation[1]);
    }

    // Write output to file or stdout
    auto writeToStdout = opt->output == "stdout";
    std::function<void(const benchmarkResult &)> writeFn;

    if (writeToStdout) {
        writeFn = [](const benchmarkResult &b) {
            std::cout << std::fixed << std::setprecision(5)
                      << b.time << "," << b.x << "," << b.y << endl;
        };
    } else {
        auto outFile = std::make_shared<std::ofstream>(opt->output);
        writeFn = [outFile](const benchmarkResult &b) mutable {
            *outFile << std::fixed << std::setprecision(5)
                     << b.time << "," << b.x << "," << b.y << endl;
        };
    }

    // Loop through the vector and write values to the appropriate target
    if (writeToStdout) {
        cout << endl << "Timing Results" << endl << "----------" << endl;
    }

    for (const auto pos: result) {
        writeFn(pos);
    }

    return 0;
}
