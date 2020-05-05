#include <getopt.h>
#include <string>

#pragma once

namespace txn {
struct Options {
    Options();
    void Parse(int argc, char** argv);

    std::string host;
    std::string user;
    std::string pass;
    std::string port;
    std::string db;

   private:
    const std::string args_ = "h:u:p:P:d:";

    const struct option long_options[6] = {{"host", required_argument, 0, 'h'},
                                           {"user", required_argument, 0, 'u'},
                                           {"pass", required_argument, 0, 'p'},
                                           {"port", required_argument, 0, 'P'},
                                           {"db", required_argument, 0, 'd'},
                                           {0, 0, 0, 0}};
};
}  // namespace txn
