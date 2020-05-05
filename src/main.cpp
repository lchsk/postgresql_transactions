#include <iostream>
#include <sstream>

#include "args.hpp"

using namespace txn;

int main(int argc, char** argv) {
    Options options;
    options.Parse(argc, argv);

    std::stringstream str;
    str << "host=" << options.host << " port=" << options.port
        << " user=" << options.user << " password=" << options.pass
        << " dbname=" << options.db;

    return 0;
}
