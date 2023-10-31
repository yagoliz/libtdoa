// SPDX-License-Identifier: Apache-2.0
//
// Copyright (c) 2023 Yago Lizarribar

#include <atomic>
#include <csignal>
#include <iostream>

#include <boost/program_options.hpp>
#include <drogon/drogon.h>

#include "../include/TdoaLocator.hh"
#include "../include/Receiver.hh"

namespace po = boost::program_options;
using namespace drogon;

// Signal handling
std::atomic<bool> shutdownFlag(false);

void signalHandler(int signum) {
    shutdownFlag.store(true);
}

// Tools for command-line parsing
struct DrogonOptions {
    std::string api_endpoint;
    std::string ip_address;
    std::string log_path;
    int port = 8095;
    int threadNum = 4;
};

int parse_commandline(int argc, char **argv, DrogonOptions &opt) {

    po::options_description desc("TdoaCLI. Command-Line utility to solve TDOA problems.\nAllowed options:");
    desc.add_options()
            ("help,h",
                    "produce help message")
            ("port,p", po::value<int>(&opt.port)->default_value(8095),
                    "Port number")
            ("ip-address,i", po::value<std::string>(&opt.ip_address)->default_value("0.0.0.0"),
                    "IP address to listen to")
            ("api-endpoint,e", po::value<std::string>(&opt.api_endpoint)->default_value("/locate"),
                    "Where to create the localization API endpoint")
            ("log-path,l", po::value<std::string>(&opt.log_path)->default_value("/tmp"),
                    "Logging path")
            ("thread-num,t", po::value<int>(&opt.threadNum)->default_value(8),
                    "Number of threads for the server");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // Help text
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    return 0;
}

// Meat of the function
int main(int argc, char **argv) {
    // Register our signal handler
    signal(SIGINT, signalHandler);

    app().getLoop()->runEvery(1000,
                              []() {
        if (shutdownFlag.load()) {
            LOG_INFO << "Exiting process...\n";
            app().quit();
        }
    });

    // Option parsing
    auto drogon_options = std::make_shared<DrogonOptions>();
    if (parse_commandline(argc, argv, *drogon_options)) {
        return 1;
    }

    // For now, we only need this endpoint
    app().registerHandler(
            drogon_options->api_endpoint,
            [](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

                // Get JSON from request
                auto obj = req->getJsonObject();
                if (obj) {
                    // Let's process the optimization method
                    int method = 1; // 1 is for LLS; 2 is for NLLS
                    if (obj->isMember("method")) {
                        auto t = (*obj)["method"].asInt();
                        if (t != 1 and t != 2) {
                            auto resp = HttpResponse::newHttpResponse();
                            resp->setStatusCode(k400BadRequest);
                            resp->setBody("Invalid optimization method. Valid options are: "
                                          "1 (for Least Squares), 2 (for Non-Linear Least Squares)\n");
                            callback(resp);
                        } else {
                            method = t;
                            LOG_INFO << "Method set to " << method << "\n";
                        }
                    } else {
                        LOG_WARN << "No method specified. Defaulting to 1 (Least Squares)\n";
                    }

                    // Get measurements
                    Json::Value result;
                    result["method"] = method;
                    Json::Value collections;
                    int k = 0;
                    if (obj->isMember("measurements")) {
                        auto mroot = (*obj)["measurements"];
                        for (const auto &measurement : mroot) {
                            std::vector<tdoapp::Receiver> r;

                            for (const auto &values : measurement) {
                                if (values.size() != 3) {
                                    LOG_WARN << "Wrong measurement file. Size of the file was: " << values.size() << ".\n";
                                    auto resp = HttpResponse::newHttpResponse();
                                    resp->setStatusCode(k400BadRequest);
                                    resp->setBody("Wrong measurement file. Each measurement must contain: "
                                                  "X, Y coordinates and timestamp.\n");
                                    callback(resp);
                                }
                                r.emplace_back(values[0].asDouble(),values[1].asDouble(),values[2].asDouble());
                            }

                            // Run the optimization routines
                            LOG_INFO << "Starting initial guess via Least Squares\n";
                            auto init = tdoapp::initialGuess(r);
                            Json::Value p;
                            if (method == 2) {
                                LOG_INFO << "Starting Non-Linear optimization\n";
                                auto nlls = tdoapp::nonlinearOptimization(r, init);
                                p["x"] = nlls[0]; p["y"] = nlls[1];
                            } else {
                                p["x"] = init[0]; p["y"] = init[1];
                            }

                            LOG_INFO << "Finished computing position" << k << "\n";
                            collections[k] = p;
                            k++;
                        }
                        result["results"] = collections;
                        auto resp = HttpResponse::newHttpJsonResponse(result);
                        callback(resp);

                    } else {
                        LOG_WARN << "File does not contain any measurement field.\n";
                        auto resp = HttpResponse::newHttpResponse();
                        resp->setStatusCode(k400BadRequest);
                        resp->setBody("File does not contain any measurements. Please make sure to put all your "
                                      "measurements in the 'measurements' field of the request.\n");
                        callback(resp);
                    }

                } else {
                    LOG_WARN << "Could not find a JSON object with the measurement information\n";
                    auto resp = HttpResponse::newHttpResponse();
                    resp->setStatusCode(k400BadRequest);
                    resp->setBody("Could not find a JSON object with the measurement information\n");
                    callback(resp);
                }
            },
            {Post});

    LOG_INFO << "Started application with the following parameters: ";
    LOG_INFO << "\t - IP address: " << drogon_options->ip_address;
    LOG_INFO << "\t - Port number: " << drogon_options->port;
    LOG_INFO << "\t - Number of threads: " << drogon_options->threadNum;
    LOG_INFO << "\t - Logging path: " << drogon_options->log_path;

    // Main app loop
    app().setLogPath(drogon_options->log_path)
            .setLogLevel(trantor::Logger::kInfo)
            .addListener(drogon_options->ip_address, drogon_options->port)
            .setThreadNum(drogon_options->threadNum)
            .run();

    return 0;
}
