#include <mutex>
#include <stack>

#include <pqxx/pqxx>

#pragma once

namespace txn {
class ConnectionPool {
   public:
    ConnectionPool(int connections, const std::string& connection);

    bool IsAvailable() const;

    void ReturnConnection(std::shared_ptr<pqxx::connection>);
    std::shared_ptr<pqxx::connection> GetConnection();

   private:
    void Lock();
    void Unlock();

    std::stack<std::shared_ptr<pqxx::connection>> pool;
    std::mutex mutex;
};
}  // namespace txn
