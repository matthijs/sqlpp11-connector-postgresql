sqlpp11-connector-postgresql
============================

PostgreSQL connector for sqlpp11 library

License:
-------------
sqlpp11 is distributed under the [BSD 2-Clause License](https://github.com/matthijs/sqlpp11-connector-postgresql/blob/master/LICENSE).

Status:
-------------
[![Build Status](https://travis-ci.org/matthijs/sqlpp11-connector-postgresql.svg?branch=master)](https://travis-ci.org/matthijs/sqlpp11-connector-postgresql?branch=master)
[![Build status](https://ci.appveyor.com/api/projects/status/bmor62aunb03hoeg/branch/master?svg=true)](https://ci.appveyor.com/project/matthijs/sqlpp11-connector-postgresql)
[![codecov](https://codecov.io/gh/matthijs/sqlpp11-connector-postgresql/branch/master/graph/badge.svg)](https://codecov.io/gh/matthijs/sqlpp11-connector-postgresql)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/ad98471cab324e1093f9554b88e90004)](https://www.codacy.com/app/matthijs_2/sqlpp11-connector-postgresql)

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

The library supports also 'ON CONFLICT'. This way INSERT or UPDATE can be implemented. To use it one can use it with the following code:
```c++
auto config = std::make_shared<sqlpp::postgresql::connection_config>();
config->host = "127.0.0.1";
config->user = "someuser";
config->password = "some-random-password";
config->dbname = "somedb";

sqlpp::postgresql::connection db(config);

TabFoo foo;
db(insert_into(foo).default_values().on_conflict().do_nothing());
db(insert_into(foo).default_values().on_conflict(foo.id).do_nothing());
db(insert_into(foo).default_values().on_conflict(foo.id).do_update(foo.name = "some data", foo.hasFun = true);
db(insert_into(foo).default_values().on_conflict(foo.id).do_update(foo.name = "some data", foo.hasFun = true).where(foo.hasFun == false));
```
The only limitation in on_conflict() is the conflict_target. Only a column is supported in the conflict_target. If there is need for a more sophisticated conflict_target please create an issue.

Connection configuration
========================
You can use all possible authentication options that are available in PostgreSQL. See [here](https://www.postgresql.org/docs/10/static/libpq-connect.html#LIBPQ-CONNSTRING) for more information about the options.
