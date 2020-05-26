#include "pool.hpp"

namespace txn {
ConnectionPool::ConnectionPool(int connections, const std::string& connection) {
    for (int i = 0; i < connections; i++) {
        pool.push(
            std::make_shared<pqxx::connection>(pqxx::connection(connection)));
    }
}

bool ConnectionPool::IsAvailable() const {
    return !pool.empty();
}

void ConnectionPool::Lock() {
    mutex.lock();
}
void ConnectionPool::Unlock() {
    mutex.unlock();
}

void ConnectionPool::ReturnConnection(std::shared_ptr<pqxx::connection> conn) {
    Lock();
    pool.push(conn);
    Unlock();
}

std::shared_ptr<pqxx::connection> ConnectionPool::GetConnection() {
    Lock();
    std::shared_ptr<pqxx::connection> c = pool.top();
    pool.pop();
    Unlock();

    return c;
}
}  // namespace txn
