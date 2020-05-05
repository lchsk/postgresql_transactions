#include "args.hpp"

namespace txn {
Options::Options()
    : host("localhost"),
      user("postgres"),
      pass("postgres"),
      port("5432"),
      db("5432") {}

void Options::Parse(int argc, char** argv) {
    int c;
    while (true) {
        int option_index = 0;
        c = getopt_long(argc, argv, args_.c_str(), long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'h':
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
        }
    }
}
}  // namespace txn
