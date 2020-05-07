#include <iostream>
#include <sstream>

#include <pqxx/pqxx>

#include "args.hpp"

using namespace txn;

int main(int argc, char** argv) {
    Options options;
    options.Parse(argc, argv);

    std::stringstream str;
    str << "host=" << options.host << " port=" << options.port
        << " user=" << options.user << " password=" << options.pass
        << " dbname=" << options.db;

    if (options.usage) {
        options.Usage();
        return 0;
    }

    if (options.test_connection) {
        std::cout << "Testing connection..." << std::endl;
        std::cout << str.str() << std::endl;
        pqxx::connection conn(str.str());
        std::cout << "Ok" << std::endl;

        return 0;
    }

    return 0;
}
