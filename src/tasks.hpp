#include <mutex>
#include <unordered_map>

#include <pqxx/pqxx>

#include "args.hpp"
#include "pool.hpp"
#include "timer.hpp"

#pragma once

namespace txn {
struct Task {
    Task(const Options& options);
    void Execute();
    void SetUp();

    std::string GetIsolationLevel() const {
        switch (isolation) {
            case 0:
                return "read_committed";
            case 1:
                return "repeatable_read";
            case 2:
                return "serializable";
        };

        return "unknown";
    }

   private:
    void SimpleInsert(std::shared_ptr<pqxx::connection> conn);

    void UpdateSingleRow(std::shared_ptr<pqxx::connection> conn);
    void UpdateManyRows(std::shared_ptr<pqxx::connection> conn);

    void SelectForUpdateSingleRow(std::shared_ptr<pqxx::connection> conn);
    void SelectForUpdateManyRows(std::shared_ptr<pqxx::connection> conn);
    void SelectForUpdateSkipLocked(std::shared_ptr<pqxx::connection> conn);
    void SelectForUpdateSkipLockedMany(std::shared_ptr<pqxx::connection> conn);
    void SelectForUpdateWithFK(std::shared_ptr<pqxx::connection> conn);

    void SelectSingle(std::shared_ptr<pqxx::connection> conn);
    void SelectMany(std::shared_ptr<pqxx::connection> conn);

    void SetUpData();

    const static pqxx::isolation_level isolation =
        pqxx::isolation_level::serializable;

    std::unique_ptr<ConnectionPool> pool;

    std::unordered_map<
        std::string,
        std::function<void(Task&, std::shared_ptr<pqxx::connection>)> >
        tasks;

    const Options& options;
    std::mutex mutex;

    Timer timer;
};
}  // namespace txn
