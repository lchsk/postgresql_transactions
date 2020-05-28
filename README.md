# PostgreSQL concurrent transactions benchmark

Benchmark a number of different operations in Postgres. It simulates a concurrent environment with different transaction isolation levels.

- Article describing the results of this benchmark: https://lchsk.com/benchmarking-concurrent-operations-in-postgresql.html

- Context about transactions and concurrent behavior in PostgreSQL: https://lchsk.com/database-transactions-concurrency-isolation-levels-and-postgresql.html


## Application

It's written in C++ and user pqxx library to connect to Postgres. It requires C++17 compiler (tested with gcc 10). It's been tested with PostgreSQL 12.3.

It has a number of parameters to change its behavior. You can run it with `--help` options to see them all.

## Parameters

`--threads` option tells the program how many threads it should run. By default, the application will open 10 connections to Postgres (it can be changed with `--connections`) and run threads to simulate the concurrent access to the database. At the end of a run, it prints the timings for each operation.

To gather benchmarks it's useful to run it with `--repetitions [n]` which will run every test `n` times and compute average timings.

## Isolation levels

The transaction isolation level used in tests must be changed in the source code (`src/tasks.hpp` - `isolation` constant). Available options include: `read_committed`, `repeatable_read`, and `serializable`.

## Compilation

Make sure you have `pqxx` installed (https://github.com/jtv/libpqxx), I've tested it with version 7.07.
You also need a compiler supporting at least C++17, I've tested it with gcc 10.

Run `make` in the main directory. That should give you a binary in `./bin/txn`.

## Running tests

You need a Postgres database. You can spin one up with `docker-compose` by running

```
docker-compose up
```

You can quickly open shell with `./open_shell.sh`.

A quick test can be run with `./bin/txn` which should run a test with 100 threads.
You can increase the number of threads with `--threads` and repeat tests with `--repetitions`. By default, it uses 10 connections to Postgres, this can be changed with `--connections`.

E.g. to run 500 threads with 5 connections and repeat each test 3 times:

```
./bin/txn --threads 500 --repetitions 3 --connections 5 
```
