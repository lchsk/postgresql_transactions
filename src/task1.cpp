#include <iostream>
#include <thread>
#include <memory>

#include "task1.hpp"

namespace txn {
void Task::SimpleInsert(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    db.exec("insert into A (id) values(1)");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::UpdateSingleRow(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    db.exec("update B set value = value + 1 where id = 1");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::UpdateManyRows(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    char query[100];

    int random = rand() % options.number_of_rows;

    sprintf(query, "update C set value = value + 1 where id = %d", random);
    db.exec(query);
    db.commit();

    pool->ReturnConnection(conn);
}

Task::Task(const Options& options) : options(options) {
    pool = std::make_unique<ConnectionPool>(options.connections,
                                            options.ConnectionString());
    SetUpData();
}

void Task::SetUpData() {
    auto c = pool->GetConnection();

    pqxx::transaction txn(*c);
    txn.exec("insert into B (id, value, notes) values(1, 0, 'b')");
    txn.commit();

    for (int i = 0; i < options.number_of_rows; i++) {
        pqxx::transaction txn(*c);
        char query[100];
        sprintf(query, "insert into C (id, value) values(%d, 0)", i);
        txn.exec(query);
        txn.commit();
    }

    pool->ReturnConnection(c);
}

void Task::Execute() {
    std::vector<std::thread> threads(options.threads);

    timer.Record("0");

    for (int i = 0; i < options.threads;) {
        if (pool->IsAvailable()) {
            auto c = pool->GetConnection();

            threads[i] = std::thread(&Task::SimpleInsert, this, c);

            i++;
        }
    }

    for (int i = 0; i < options.threads; i++) {
        threads[i].join();
    }

    timer.Record("1");

    for (int i = 0; i < options.threads;) {
        if (pool->IsAvailable()) {
            auto c = pool->GetConnection();

            threads[i] = std::thread(&Task::UpdateSingleRow, this, c);

            i++;
        }
    }

    for (int i = 0; i < options.threads; i++) {
        threads[i].join();
    }

    timer.Record("2");

    for (int i = 0; i < options.threads;) {
        if (pool->IsAvailable()) {
            auto c = pool->GetConnection();

            threads[i] = std::thread(&Task::UpdateManyRows, this, c);

            i++;
        }
    }

    for (int i = 0; i < options.threads; i++) {
        threads[i].join();
    }

    timer.Record("3");

    auto time_task_1 = timer.GetMs("0", "1");
    auto time_task_2 = timer.GetMs("1", "2");
    auto time_task_3 = timer.GetMs("2", "3");

    std::cout << "\n";

    std::cout << "Simple insert time: " << time_task_1 << " ms" << std::endl;

    std::cout << "Single row update time: " << time_task_2 << " ms"
              << std::endl;

    std::cout << "Many rows update time: " << time_task_3 << " ms" << std::endl;
}
}
