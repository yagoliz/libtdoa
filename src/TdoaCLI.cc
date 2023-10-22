// SPDX-License-Identifier: Apache-2.0
//
// Copyright (c) 2023 Yago Lizarribar

#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>
#include <nlohmann/json.hpp>

#include "../include/Receiver.h"
#include "../include/TdoaLocator.h"

using std::cout;
using std::cerr;
using std::endl;

namespace po = boost::program_options;

using nlohmann::json;

struct options {
    int optimization_level = 1;
    std::string gNBFile;
    std::string TOAFile;
    std::string output;
};

int parse_commandline(int argc, char** argv, options &opt) {

    po::options_description desc("TdoaCLI. Command-Line utility to solve TDOA problems.\nAllowed options:");
    desc.add_options()
            ("help,h", "produce help message")
            ("gNB,g", po::value<std::string>(), "JSON file with gNB positions")
            ("TOA,t", po::value<std::string>(), "JSON file with TOA per gNB")
            ("method", po::value<int>(&opt.optimization_level)->default_value(1), "Method to use (1: linear, 2: nonlinear). Default: 1")
            ("output", po::value<std::string>(&opt.output)->default_value("stdout"), "Where to dump the output: (stdout or file)")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // Help text
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    // gNB file is mandatory
    if (vm.count("gNB")) {
        auto gnbfile = vm["gNB"].as<std::string>();
        cout << "Selected gNB file: " << gnbfile << endl;
        opt.gNBFile = gnbfile;
    } else {
        cerr << "Must provide gNB positions" << endl;
        cerr << desc << endl;
        return 1;
    }

    // Also ToA values are mandatory
    if (vm.count("TOA")) {
        auto toafile = vm["TOA"].as<std::string>();
        cout << "Selected TOA file: " << toafile << endl;
        opt.TOAFile = toafile;
    } else {
        cerr << "Must provide TOA values" << endl;
        cerr << desc << endl;
        return 1;
    }

    // Last we select the method
    if (vm.count("method")) {
        int method = vm["method"].as<int>();
        cout << "Method was set to: "
             << method << "." << endl;
        opt.optimization_level = method;
    } else {
        cerr << "Compression level was not set." << endl;
        cout << desc << endl;
    }

    return 0;
}

int main(int argc, char** argv) {
    // Command line options
    auto opt = std::make_shared<options>();
    if (parse_commandline(argc, argv, *opt)) {
        return 1;
    }

    // Get receivers information
    std::ifstream ifs(opt->gNBFile);
    if (!ifs.is_open()) {
        cerr << "Error: Could not open gNB file" << endl;
        return 1;
    }

    auto gNBs = json::parse(ifs);
    std::vector<libtdoa::Receiver> receivers;
    if (gNBs.contains("gNBs") && gNBs["gNBs"].is_object()) {
        for (auto& [key, value] : gNBs["gNBs"].items()) {
            if (value.is_array() && value.size() == 2) {
                receivers.emplace_back(value[0].get<double>(), value[1].get<double>());
            }
        }
    } else {
        cerr << "Error parsing gNB file." << endl;
        return 1;
    }

    // Get TOA values and run optimization
    std::ifstream toafs(opt->TOAFile);
    if (!toafs.is_open()) {
        cerr << "Error: Could not open TOA file" << endl;
        return 1;
    }

    auto toas = json::parse(toafs);
    std::vector<std::array<double,2>> result;
    if (toas.is_object()) {
        for (auto& [key, value] : toas.items()) {

            // Populate the receivers with the timestamps
            if (value.is_array() && value.size() == receivers.size()) {
                for (int j = 0; j < receivers.size(); j++) {
                    receivers[j].timestamp = value[j].get<double>();
                }
            } else {
                cerr << "For key: " << key << ", the number of TOA values is " << value.size()
                    << ", but there are " << receivers.size() << " receivers..." << endl;

                return 1;
            }

            // Run the optimization routines
            auto init = libtdoa::initialGuess(receivers);

            if (opt->optimization_level == 2) {
                auto nlls = libtdoa::nonlinearOptimization(receivers, init);
                result.emplace_back(std::array<double,2>{nlls[0], nlls[1]});
            } else {
                result.emplace_back(std::array<double,2>{init[0], init[1]});
            }
        }
    } else {
        cerr << "Error parsing TOA file." << endl;
        return 1;
    }


    // Write output to file or stdout
    auto writeToStdout = opt->output == "stdout";
    std::function<void(const std::array<double,2>&)> writeFn;

    if (writeToStdout) {
        writeFn = [](const std::array<double,2>& values) { std::cout << "X: " << values[0] << ", Y: " << values[1] << endl; };
    } else {
        std::ofstream outFile(opt->output);
        writeFn = [&outFile](const std::array<double,2>& values) { outFile << "X: " << values[0] << ", Y: " << values[1] << endl; };
    }

    // Loop through the vector and write values to the appropriate target
    if (writeToStdout) {
        cout << endl << "Positioning Results" << endl << "----------" << endl;
    }
    for (const auto pos : result) {
        writeFn(pos);
    }

    return 0;
}