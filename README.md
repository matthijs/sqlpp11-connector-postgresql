sqlpp11-connector-postgresql
============================
[![Build Status](https://travis-ci.org/matthijs/sqlpp11-connector-postgresql.svg?branch=master)](https://travis-ci.org/matthijs/sqlpp11-connector-postgresql?branch=master)

PostgreSQL connector for sqlpp11 library

Fork Info
===========

This is a fork of the sqlpp11-connector-postgresql repository by matthijs. It adds the following features:

* impement a "dynamic loading" variant which loads the libpq library using dlopen / LoadLibrary. Link agsinst the 
  sqlpp-postgresql-dynamic.so library (and other connectors, e.g. sqlpp-sqlite3-dynamic) to support both databases 
  in one binary without requiring all the different database client libraries to be installed.

* implement sqlpp11 std::chrono / "Hinnant Date" based date/time handling

* implement support for setting isolation levels for transactions

* implements missing connection::execute() function

* implemented "integration tests" that will work with a local PostgreSQL DB where a database with the name "$USER" 
  is present and the $USER can login via "peer" authentication. Use "cmake -DENABLE\_TESTS=True ..." to build tests.

* some minor cleanups and optimizations
