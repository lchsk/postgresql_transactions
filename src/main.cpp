#include <fstream>
#include <iostream>
#include <sstream>

#include <pqxx/pqxx>

#include "args.hpp"
#include "task1.hpp"

using namespace txn;

int main(int argc, char** argv) {
    Options options;
    options.Parse(argc, argv);

    if (!options.keep_schema) {
        std::fstream schema;
        schema.open("./sql/schema.sql");

        if (schema.is_open()) {
            std::stringstream sql;
            std::string line;

            while (std::getline(schema, line)) {
                sql << line;
            }

            schema.close();

            std::cout << "Resetting the schema\n";
            pqxx::connection conn(options.ConnectionString());
            pqxx::nontransaction db(conn);
            db.exec(sql);
            std::cout << "Schema reset\n";
        }
    }

    if (options.usage) {
        options.Usage();
        return 0;
    }

    if (options.test_connection) {
        std::cout << "Testing connection..." << std::endl;
        std::cout << options.ConnectionString() << std::endl;
        pqxx::connection conn(options.ConnectionString());
        std::cout << "Ok" << std::endl;

        return 0;
    }

    std::cout << "Connections: " << options.connections << std::endl;
    std::cout << "Threads: " << options.threads << std::endl;
    std::cout << "Number of rows: " << options.number_of_rows << std::endl;

    Task t(options);
    t.Execute();

    return 0;
}
