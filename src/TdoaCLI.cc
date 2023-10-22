// SPDX-License-Identifier: Apache-2.0
//
// Copyright (c) 2023 Yago Lizarribar

#include <iostream>
#include <boost/program_options.hpp>
#include <nlohmann/json.hpp>

#include "../include/Receiver.h"
#include "../include/TdoaLocator.h"

using std::cout;
using std::cerr;
using std::endl;

namespace po = boost::program_options;

struct options {
    int optimization_level = 1;
    std::string gNBFile;
    std::string TOAFile;
};

int parse_commandline(int argc, char** argv, options &opt) {

    po::options_description desc("TdoaCLI. Command-Line utility to solve TDOA problems.\nAllowed options:");
    desc.add_options()
            ("help,h", "produce help message")
            ("gNB,g", po::value<std::string>(), "JSON file with gNB positions")
            ("TOA,t", po::value<std::string>(), "JSON file with TOA per gNB")
            ("method", po::value<int>(&opt.optimization_level)->default_value(1), "Method to use (1: linear, 2: nonlinear). Default: 1")
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
    if (vm.count("gNB")) {
        auto toafile = vm["gNB"].as<std::string>();
        cout << "Selected TOA file: " << toafile << endl;
        opt.gNBFile = toafile;
    } else {
        cerr << "Must provide TOA values" << endl;
        cerr << desc << endl;
        return 1;
    }

    // Last we select the method
    if (vm.count("method")) {
        int method = vm["method"].as<int>();
        cout << "Method was set to: "
             << method << ".";
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

    return 0;
}

