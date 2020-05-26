#include <functional>
#include <iostream>
#include <memory>
#include <thread>

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

void Task::SelectForUpdateSingleRow(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    db.exec("select value from B where id = 1 for update;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateSkipLocked(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    db.exec("select value from C limit 1 for update skip locked;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateSkipLockedMany(
    std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    db.exec("select value from C limit 10 for update skip locked;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateManyRows(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    db.exec("select value from B for update;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectSingle(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    db.exec("select value from B where id = 1;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectMany(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction db(*conn);
    db.exec("select value from B where id = 1;");
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

    tasks["simple_insert"] = &Task::SimpleInsert;
    tasks["update_single_row"] = &Task::UpdateSingleRow;
    tasks["update_many_rows"] = &Task::UpdateManyRows;
    tasks["select_for_update_single_row"] = &Task::SelectForUpdateSingleRow;
    tasks["select_for_update_many_rows"] = &Task::SelectForUpdateManyRows;
    tasks["select_for_update_skip_locked"] = &Task::SelectForUpdateSkipLocked;
    tasks["select_for_update_skip_locked_many"] =
        &Task::SelectForUpdateSkipLockedMany;
    tasks["select_single"] = &Task::SelectSingle;
    tasks["select_many"] = &Task::SelectMany;

    SetUpData();
}

void Task::SetUpData() {
    auto c = pool->GetConnection();

    pqxx::transaction txn(*c);
    txn.exec("insert into B (id, value) values(1, 0)");
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

    for (auto& [name, func] : tasks) {
        std::cout << "Running " << name << std::endl;
        timer.Record(name + "_start");

        for (int i = 0; i < options.threads;) {
            if (pool->IsAvailable()) {
                auto c = pool->GetConnection();
                threads[i] = std::thread(func, std::ref(*this), c);

                i++;
            }
        }

        for (int i = 0; i < options.threads; i++) {
            threads[i].join();
        }

        timer.Record(name + "_end");
    }

    // Get summary

    auto t_simple_insert =
        timer.GetMs("simple_insert_start", "simple_insert_end");
    auto t_update_single_row =
        timer.GetMs("update_single_row_start", "update_single_row_end");
    auto t_update_many_rows =
        timer.GetMs("update_many_rows_start", "update_many_rows_end");
    auto t_sel_for_upd_single =
        timer.GetMs("select_for_update_single_row_start",
                    "select_for_update_single_row_end");
    auto t_sel_for_upd_many = timer.GetMs("select_for_update_many_rows_start",
                                          "select_for_update_many_rows_end");
    auto t_sel_for_upd_skip = timer.GetMs("select_for_update_skip_locked_start",
                                          "select_for_update_skip_locked_end");
    auto t_sel_for_upd_skip_many =
        timer.GetMs("select_for_update_skip_locked_many_start",
                    "select_for_update_skip_locked_many_end");
    auto t_sel_single = timer.GetMs("select_single_start", "select_single_end");
    auto t_sel_many = timer.GetMs("select_many_start", "select_many_end");

    std::cout << "\n";

    std::cout << "Simple insert time: " << t_simple_insert << " ms"
              << std::endl;

    std::cout << "Single row update time: " << t_update_single_row << " ms"
              << std::endl;

    std::cout << "Many rows update time: " << t_update_many_rows << " ms"
              << std::endl;

    std::cout << "Select for update single row: " << t_sel_for_upd_single
              << " ms" << std::endl;
    std::cout << "Select for update many rows: " << t_sel_for_upd_many << " ms"
              << std::endl;
    std::cout << "Select for update skip locked: " << t_sel_for_upd_skip
              << " ms" << std::endl;
    std::cout << "Select for update many rows skip locked: "
              << t_sel_for_upd_skip_many << " ms" << std::endl;
    std::cout << "Select single row: " << t_sel_single << " ms" << std::endl;
    std::cout << "Select many rows: " << t_sel_many << " ms" << std::endl;
}
}  // namespace txn
