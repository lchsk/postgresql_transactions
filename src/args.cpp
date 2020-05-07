#include <iostream>

#include "args.hpp"

namespace txn {
Options::Options()
    : host("localhost"),
      user("txn"),
      pass("txn"),
      port("1234"),
      db("txn_db"),
      test_connection(0),
      usage(0) {}

void Options::Usage() const {
    const std::string usage =
        "Usage:\n"
        "\t--host postgres host\n"
        "\t--user postgres user\n"
        "\t--pass postgres password\n"
        "\t--port postgres port\n"
        "\t--db postgres database\n"
        "\t--test-connection check connection to postgres and quit\n";

    std::cout << usage << std::endl;
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
            case 'h':
                usage = 1;
                break;
        }
    }
}
}  // namespace txn
