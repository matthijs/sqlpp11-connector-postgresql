sqlpp11-connector-postgresql
============================

PostgreSQL connector for sqlpp11 library

License:
-------------
sqlpp11 is distributed under the [BSD 2-Clause License](https://github.com/matthijs/sqlpp11-connector-postgresql/blob/master/LICENSE).

Status:
-------------
Branch / Compiler | gcc 4.8 | MSVC 2015/2017 | Code coverage
------------------|---------|-------------------------------
master | [![Build Status](https://travis-ci.org/matthijs/sqlpp11-connector-postgresql.svg?branch=master)](https://travis-ci.org/matthijs/sqlpp11-connector-postgresql?branch=master) | [![Build status](https://ci.appveyor.com/api/projects/status/bmor62aunb03hoeg/branch/master?svg=true)](https://ci.appveyor.com/project/matthijs/sqlpp11-connector-postgresql) | [![Coverage Status](https://coveralls.io/repos/github/matthijs/sqlpp11-connector-postgresql/badge.svg?branch=master)](https://coveralls.io/github/matthijs/sqlpp11-connector-postgresql?branch=master)
develop | [![Build Status](https://travis-ci.org/matthijs/sqlpp11-connector-postgresql.svg?branch=develop)](https://travis-ci.org/matthijs/sqlpp11-connector-postgresql?branch=develop) | [![Build status](https://ci.appveyor.com/api/projects/status/bmor62aunb03hoeg/branch/develop?svg=true)](https://ci.appveyor.com/project/matthijs/sqlpp11-connector-postgresql) | [![Coverage Status](https://coveralls.io/repos/github/matthijs/sqlpp11-connector-postgresql/badge.svg?branch=master)](https://coveralls.io/github/matthijs/sqlpp11-connector-postgresql?branch=develop)

Examples
========

An example on how to use this library
```c++
auto config = std::make_shared<sqlpp::postgresql::connection_config>();
config->host = "127.0.0.1";
config->user = "someuser";
config->password = "some-random-password";
config->dbname = "somedb";

sqlpp::postgresql::connection db(config);

TabFoo foo;
for(const auto& row: db(select(foo.name, foo.hasFun).from(foo).where(foo.id > 17 and foo.name.like("%bar%"))) {
  std::cerr << row.name << std::endl;
}
```

Connection configuration
========================
You can use all possible authentication options that are available in PostgreSQL. See [here](https://www.postgresql.org/docs/10/static/libpq-connect.html#LIBPQ-CONNSTRING) for more information about the options.
