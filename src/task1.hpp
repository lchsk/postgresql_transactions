#include <mutex>

#include <pqxx/pqxx>

#include "pool.hpp"
#include "args.hpp"
#include "timer.hpp"

#pragma once

namespace txn {
struct Task {
    Task(const Options& options);
    void Execute();

   private:
    void SimpleInsert(std::shared_ptr<pqxx::connection> conn);
    void UpdateSingleRow(std::shared_ptr<pqxx::connection> conn);
    void UpdateManyRows(std::shared_ptr<pqxx::connection> conn);

    void SetUpData();

    std::unique_ptr<ConnectionPool> pool;

    const Options& options;
    std::mutex mutex;

    Timer timer;
};
}
