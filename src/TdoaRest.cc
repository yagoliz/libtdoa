// SPDX-License-Identifier: Apache-2.0
//
// Copyright (c) 2023 Yago Lizarribar

#include <boost/program_options.hpp>
#include <drogon/drogon.h>

using namespace drogon;

int main() {
    app().registerHandler("/locate",
                          [](const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback,
                             const std::string &name) {
                              Json::Value json;
                              json["result"] = "ok";
                              json["message"] = std::string("hello,") + name;
                              auto resp = HttpResponse::newHttpJsonResponse(json);
                              callback(resp);
                          },
                          {Get, "LoginFilter"});

    app().setLogPath("./")
            .setLogLevel(trantor::Logger::kWarn)
            .addListener("0.0.0.0", 80)
            .setThreadNum(16)
            .run();
}
