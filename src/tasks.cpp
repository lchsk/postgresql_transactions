#include <functional>
#include <iostream>
#include <memory>
#include <thread>

#include "tasks.hpp"

namespace txn {
void Task::SimpleInsert(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction<isolation> db(*conn);
    db.exec("insert into A (id) values(1)");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::UpdateSingleRow(std::shared_ptr<pqxx::connection> conn) {
    // Retry transaction if it fails due to serialization failures
    // at higher isolation levels.

    while (true) {
        pqxx::transaction<isolation> db(*conn);
        try {
            db.exec("update B set value = value + 1 where id = 1");
            db.commit();
            break;
        } catch (const pqxx::serialization_failure&) {
            db.abort();
            continue;
        } catch (const pqxx::failure& e) {
            std::cout << "update_single_row fail" << e.what() << std::endl;
            db.abort();
            break;
        }
    }

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateSingleRow(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction<isolation> db(*conn);
    db.exec("select value from B where id = 1 for update;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateSkipLocked(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction<isolation> db(*conn);
    db.exec("select value from C limit 1 for update skip locked;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateSkipLockedMany(
    std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction<isolation> db(*conn);
    db.exec("select value from C limit 10 for update skip locked;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateManyRows(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction<isolation> db(*conn);
    db.exec("select value from B for update;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateAllFK(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction<isolation> db(*conn);
    db.exec("select * from E for update;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectForUpdateWithFK(std::shared_ptr<pqxx::connection> conn) {
    const int random = rand() % 2;

    pqxx::transaction<isolation> db(*conn);

    if (random == 0) {
        db.exec("select * from E for update;");
    } else if (random == 1) {
        try {
            db.exec("update D set id = id + 1000;");
        } catch (const pqxx::failure&) {
        }
    }

    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectSingle(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction<isolation> db(*conn);
    db.exec("select value from b where id = 1;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::SelectMany(std::shared_ptr<pqxx::connection> conn) {
    pqxx::transaction<isolation> db(*conn);
    db.exec("select id from a;");
    db.commit();

    pool->ReturnConnection(conn);
}

void Task::UpdateRandomRow(std::shared_ptr<pqxx::connection> conn) {
    const int random = rand() % options.number_of_rows;

    char query[100];
    sprintf(query, "update C set value = value + 1 where id = %d", random);

    // Retry transaction if it fails due to serialization failures
    // at higher isolation levels.

    while (true) {
        pqxx::transaction<isolation> db(*conn);
        try {
            db.exec(query);
            db.commit();
            break;
        } catch (const pqxx::serialization_failure&) {
            db.abort();
            continue;
        } catch (const pqxx::failure& e) {
            std::cout << "update_random_row fail" << e.what() << std::endl;
            db.abort();
            break;
        }
    }

    pool->ReturnConnection(conn);
}

Task::Task(const Options& options) : options(options) {
    tasks["simple_insert"] = &Task::SimpleInsert;
    tasks["update_single_row"] = &Task::UpdateSingleRow;
    tasks["update_random_row"] = &Task::UpdateRandomRow;
    tasks["select_for_update_single_row"] = &Task::SelectForUpdateSingleRow;
    tasks["select_for_update_many_rows"] = &Task::SelectForUpdateManyRows;
    tasks["select_for_update_skip_locked"] = &Task::SelectForUpdateSkipLocked;
    tasks["select_for_update_skip_locked_many"] =
        &Task::SelectForUpdateSkipLockedMany;
    tasks["select_for_update_with_fk"] = &Task::SelectForUpdateWithFK;
    tasks["select_for_update_all_fk"] = &Task::SelectForUpdateAllFK;
    tasks["select_single"] = &Task::SelectSingle;
    tasks["select_many"] = &Task::SelectMany;

    for (auto& [name, _] : tasks) {
        avg_timings[name] = 0;
    }
}
void Task::SetUp() {
    pool = std::make_unique<ConnectionPool>(options.connections,
                                            options.ConnectionString());

    SetUpData();
}

void Task::SetUpData() {
    auto c = pool->GetConnection();

    pqxx::transaction txn(*c);
    txn.exec("insert into B (id, value, notes) values(1, 100, 'a')");
    txn.exec("insert into B (id, value, notes) values(2, 200, 'a')");
    txn.exec("insert into B (id, value, notes) values(3, 300, 'b')");
    txn.exec("insert into B (id, value, notes) values(4, 400, 'b')");
    txn.commit();

    for (int i = 0; i < options.number_of_rows; i++) {
        pqxx::transaction txn(*c);
        char query[100];
        sprintf(query, "insert into C (id, value) values(%d, 0)", i);
        txn.exec(query);
        txn.commit();
    }

    for (int i = 0; i < options.number_of_rows; i++) {
        pqxx::transaction txn(*c);

        char query_1[100];
        char query_2[100];

        sprintf(query_1, "insert into D (id, value) values(%d, 0)", i);
        sprintf(query_2, "insert into E (id, value, d_id) values(%d, 1, %d)", i,
                i);

        txn.exec(query_1);
        txn.exec(query_2);

        txn.commit();
    }

    pool->ReturnConnection(c);
}

void Task::Execute() {
    std::vector<std::thread> threads(options.threads);

    for (auto& [name, func] : tasks) {
        if (name != options.task) {
            continue;
        }

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
    auto t_update_random_row =
        timer.GetMs("update_random_row_start", "update_random_row_end");
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
    auto t_sel_for_upd_with_fk = timer.GetMs("select_for_update_with_fk_start",
                                             "select_for_update_with_fk_end");
    auto t_sel_for_upd_all_fk = timer.GetMs("select_for_update_all_fk_start",
                                            "select_for_update_all_fk_end");
    auto t_sel_single = timer.GetMs("select_single_start", "select_single_end");
    auto t_sel_many = timer.GetMs("select_many_start", "select_many_end");

    avg_timings["simple_insert"] += t_simple_insert;
    avg_timings["update_single_row"] += t_update_single_row;
    avg_timings["update_random_row"] += t_update_random_row;
    avg_timings["select_for_update_single_row"] += t_sel_for_upd_single;
    avg_timings["select_for_update_many_rows"] += t_sel_for_upd_many;
    avg_timings["select_for_update_skip_locked"] += t_sel_for_upd_skip;
    avg_timings["select_for_update_skip_locked_many"] +=
        t_sel_for_upd_skip_many;
    avg_timings["select_for_update_with_fk"] += t_sel_for_upd_with_fk;
    avg_timings["select_for_update_all_fk"] += t_sel_for_upd_all_fk;
    avg_timings["select_single"] += t_sel_single;
    avg_timings["select_many"] += t_sel_many;

    std::cout << "\n";

    std::cout << "Simple insert time: " << t_simple_insert << " ms"
              << std::endl;

    std::cout << "Single row update time: " << t_update_single_row << " ms"
              << std::endl;

    std::cout << "Random row update time: " << t_update_random_row << " ms"
              << std::endl;

    std::cout << "Select for update single row: " << t_sel_for_upd_single
              << " ms" << std::endl;
    std::cout << "Select for update many rows: " << t_sel_for_upd_many << " ms"
              << std::endl;
    std::cout << "Select for update skip locked: " << t_sel_for_upd_skip
              << " ms" << std::endl;
    std::cout << "Select for update many rows skip locked: "
              << t_sel_for_upd_skip_many << " ms" << std::endl;
    std::cout << "Select for update (50%) with foreign key: "
              << t_sel_for_upd_with_fk << " ms" << std::endl;
    std::cout << "Select for update (all) with foreign key: "
              << t_sel_for_upd_all_fk << " ms" << std::endl;
    std::cout << "Select single row: " << t_sel_single << " ms" << std::endl;
    std::cout << "Select many rows: " << t_sel_many << " ms" << std::endl;
}

void Task::PrintAvgTimings() const {
    for (auto& [name, value] : tasks) {
        std::cout << name << ": "
                  << avg_timings.at(name) /
                         static_cast<double>(options.repetitions)
                  << std::endl;
    }
}
}  // namespace txn
