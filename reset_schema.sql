#!/usr/bin/env bash

PGPASSWORD=txn psql -h localhost -U txn -h localhost -p 1234 -d txn_db -f ./sql/schema.sql
