// SPDX-License-Identifier: Apache-2.0
//
// Copyright (c) 2023 Yago Lizarribar

#include <fstream>
#include <iomanip>
#include <iostream>

#include <boost/program_options.hpp>
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
    std::string receiver_file;
    std::string output;
};

int parse_commandline(int argc, char **argv, options &opt) {

    po::options_description desc("TdoaCLI. Command-Line utility to solve TDOA problems.\nAllowed options:");
    desc.add_options()
            ("help,h", "produce help message")
            ("receiver,r", po::value<std::string>(), "JSON file with receiver positions & timestamps")
            ("method,m", po::value<int>(&opt.optimization_level)->default_value(1),
             "Method to use (1: linear, 2: nonlinear). Default: 1")
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

    // receiver file is mandatory
    if (vm.count("receiver")) {
        auto receiverfile = vm["receiver"].as<std::string>();
        cout << "Selected receiver file: " << receiverfile << endl;
        opt.receiver_file = receiverfile;
    } else {
        cerr << "Must provide receiver positions" << endl;
        cerr << desc << endl;
        return 1;
    }

    // Last we select the method
    if (vm.count("method")) {
        int method = vm["method"].as<int>();
        std::string m = method == 1 ? "Linear/Least Squares" : "Non-Linear Least Squares";
        cout << "Optimization Method was set to: " << m << "." << endl;
        opt.optimization_level = method;
    } else {
        cerr << "Method level was not set. Linear/Least Squares optimization will be run" << endl;
        cout << desc << endl;
    }

    return 0;
}

int main(int argc, char **argv) {
    // Command line options
    auto opt = std::make_shared<options>();
    if (parse_commandline(argc, argv, *opt)) {
        return 1;
    }

    // Get receivers information
    std::ifstream ifs(opt->receiver_file);
    if (!ifs.is_open()) {
        cerr << "Error: Could not open receiver file" << endl;
        return 1;
    }

    // receiver file should contain a vector with a "measurements" field
    // Inside, there should a vector with N positions to analyze
    auto receivers = json::parse(ifs);
    std::vector<std::array<double, 2>> result;
    if (receivers.contains("measurements")) {

        // Main loop over the received measurements
        for (const auto &measurement: receivers["measurements"]) {
            // Looping over how many receivers
            std::vector<tdoapp::Receiver> r;
            for (const auto &[key, values]: measurement.items()) {
                if (values.is_array() && values.size() == 3) {
                    r.emplace_back(values[0].get<double>(),
                                   values[1].get<double>(),
                                   values[2].get<double>());
                } else {
                    cerr << "Wrong format for JSON value in measurement. Expected 3-element array" << endl
                         << "The array assertion was: " << values.is_array() << ". The size was: " << values.size()
                         << "" << endl;
                }
            }

            // Run the optimization routines
            auto init = tdoapp::initialGuess(r);

            if (opt->optimization_level == 2) {
                auto nlls = tdoapp::nonlinearOptimization(r, init);
                result.emplace_back(std::array<double, 2>{nlls[0], nlls[1]});
            } else {
                result.emplace_back(std::array<double, 2>{init[0], init[1]});
            }
        }

    } else {
        cerr << "Error parsing receiver file." << endl;
        return 1;
    }

    // Write output to file or stdout
    auto writeToStdout = opt->output == "stdout";
    std::function<void(const std::array<double, 2> &)> writeFn;

    if (writeToStdout) {
        writeFn = [](const std::array<double, 2> &values) {
            std::cout << std::fixed << std::setprecision(5) << "X: " << values[0] << ", Y: " << values[1] << endl;
        };
    } else {
        auto outFile = std::make_shared<std::ofstream>(opt->output);
        writeFn = [outFile](const std::array<double, 2> &values) mutable {
            *outFile << std::fixed << std::setprecision(5) << "X: " << values[0] << ", Y: " << values[1] << endl;
        };
    }

    // Loop through the vector and write values to the appropriate target
    if (writeToStdout) {
        cout << endl << "Positioning Results" << endl << "----------" << endl;
    }
    for (const auto pos: result) {
        writeFn(pos);
    }

    return 0;
}