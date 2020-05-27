#include <getopt.h>
#include <string>

#pragma once

namespace txn {
struct Options {
    Options();
    void Parse(int argc, char** argv);
    void Usage() const;
    const std::string ConnectionString() const;

    std::string host;
    std::string user;
    std::string pass;
    std::string port;
    std::string db;

    std::string task;
    int connections;
    int threads;

    int repetitions;

    int test_connection;
    int usage;
    int keep_schema;

    // Task-specific
    int number_of_rows;

   private:
    const std::string args = "H:u:p:P:d:T:c:t:R:r:ThK";

    const struct option long_options[14] = {
        {"host", required_argument, 0, 'H'},
        {"user", required_argument, 0, 'u'},
        {"pass", required_argument, 0, 'p'},
        {"port", required_argument, 0, 'P'},
        {"db", required_argument, 0, 'd'},
        {"task", required_argument, 0, 'T'},
        {"connections", required_argument, 0, 'c'},
        {"threads", required_argument, 0, 't'},
        {"repetitions", required_argument, 0, 'r'},
        {"test-connection", no_argument, &test_connection, 'E'},
        {"help", no_argument, &usage, 'h'},
        {"keep-schema", no_argument, &keep_schema, 'K'},

        // Task-specific
        {"rows", required_argument, 0, 'R'},

        {0, 0, 0, 0}};
};
}  // namespace txn
