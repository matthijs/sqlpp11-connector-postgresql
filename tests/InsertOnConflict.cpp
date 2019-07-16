#include <iostream>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include "TabBar.h"
#include "TabFoo.h"

namespace sql = sqlpp::postgresql;

int main(int argc, char** argv)
{
  model::TabFoo foo = {};
  model::TabBar bar = {};

  auto config = std::make_shared<sql::connection_config>();

  // Test on conflict
  sql::insert_into(foo).default_values().on_conflict().do_nothing();
  /*sql::insert_into(foo).set(foo.gamma = "dsa")
    .on_conflict()
      .do_nothing();*/

  /*sql::insert_into(foo).set(foo.gamma = "dsa")
    .on_conflict()
    .do_update()
      .set(column_list...)
      .where(expressions...);*/

  return 0;
}
