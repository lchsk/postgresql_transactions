#include <iostream>
#include <sstream>

#include "args.hpp"

namespace txn {
Options::Options()
    : host("localhost"),
      user("txn"),
      pass("txn"),
      port("1234"),
      db("txn_db"),
      task(""),
      connections(10),
      threads(100),
      repetitions(1),
      test_connection(0),
      usage(0),
      keep_schema(0),

      // Task-specific
      number_of_rows(100) {}

void Options::Usage() const {
    const std::string usage =
        "Usage:\n"
        "\t--host postgres host\n"
        "\t--user postgres user\n"
        "\t--pass postgres password\n"
        "\t--port postgres port\n"
        "\t--db postgres database\n"
        "\n"
        "\t--task run a particular task\n"
        "\t--connections [int] number of connections to postgres\n"
        "\t--threads [int] number of threads to use in a test\n"
        "\t--repetitions [int] how many times each test should be ran\n"
        "\n"
        "\t--keep_schema don't recreate the test database schema\n"
        "\t--test-connection check connection to postgres and quit\n"

        "\n"
        "\tTask-specific\n"
        "\t--rows [int] Number of rows to update in UpdateManyRows test\n"

        ;

    std::cout << usage << std::endl;
}

const std::string Options::ConnectionString() const {
    std::stringstream str;
    str << "host=" << host << " port=" << port << " user=" << user
        << " password=" << pass << " dbname=" << db;

    return str.str();
}

void Options::Parse(int argc, char** argv) {
    int c;

    while (true) {
        int option_index = 0;
        c = getopt_long(argc, argv, args.c_str(), long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'H':
                host = optarg;
                break;
            case 'u':
                user = optarg;
                break;
            case 'p':
                pass = optarg;
                break;
            case 'P':
                port = optarg;
                break;
            case 'd':
                db = optarg;
                break;
            case 'T':
                task = optarg;
                break;
            case 'c':
                connections = std::atoi(optarg);
                break;
            case 't':
                threads = std::atoi(optarg);
                break;
            case 'r':
                repetitions = std::atoi(optarg);
                break;
            case 'R':
                number_of_rows = std::atoi(optarg);
                break;
            case 'h':
                usage = 1;
                break;
        }
    }
}
}  // namespace txn
