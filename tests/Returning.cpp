#include <iostream>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>

#include "TabFoo.h"
#include "TabBar.h"

namespace sql = sqlpp::postgresql;
int Returning(int, char**)
{
  model::TabFoo foo;
  model::TabBar bar;
  auto config = std::make_shared<sql::connection_config>();
  config->dbname = "sqlpp11_tests";
  try
  {
    sql::connection db(config);
  }
  catch (const sql::broken_connection&)
  {
    std::cerr << "For testing, you'll need to create a database sqlpp_postgresql" << std::endl;
    throw;
  }

  sql::connection db(config);
  try
  {
    db.execute(R"(DROP TABLE IF EXISTS tabfoo;)");
    db.execute(R"(CREATE TABLE tabfoo
                   (
                   alpha bigserial NOT NULL,
                   beta smallint,
                   gamma text,
                   c_bool boolean,
                   c_timepoint timestamp with time zone DEFAULT now(),
                   c_day date
                   ))");

    std::cout
        << db(sqlpp::postgresql::insert_into(foo).set(foo.gamma = "dsa").returning(foo.c_timepoint)).front().c_timepoint
        << std::endl;

    auto i = sqlpp::postgresql::dynamic_insert_into(db, foo).dynamic_set().returning(foo.c_timepoint);
    i.insert_list.add(foo.gamma = "blah");

    std::cout << db(i).front().c_timepoint << std::endl;

    auto updated =
        db(sqlpp::postgresql::update(foo).set(foo.beta = 0).unconditionally().returning(foo.gamma, foo.beta));
    for (const auto& row : updated)
      std::cout << "Gamma: " << row.gamma << " Beta: " << row.beta << std::endl;
  }

  catch (const sql::failure&)
  {
    return 1;
  }

  return 0;
}
