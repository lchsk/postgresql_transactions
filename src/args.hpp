#include <getopt.h>
#include <string>

#pragma once

namespace txn {
struct Options {
    Options();
    void Parse(int argc, char** argv);
    void Usage() const;

    std::string host;
    std::string user;
    std::string pass;
    std::string port;
    std::string db;

    int test_connection;
    int usage;

   private:
    const std::string args = "H:u:p:P:d:Th";

    const struct option long_options[8] = {
        {"host", required_argument, 0, 'H'},
        {"user", required_argument, 0, 'u'},
        {"pass", required_argument, 0, 'p'},
        {"port", required_argument, 0, 'P'},
        {"db", required_argument, 0, 'd'},
        {"test-connection", no_argument, &test_connection, 'T'},
        {"help", no_argument, &usage, 'h'},
        {0, 0, 0, 0}};
};
}  // namespace txn
