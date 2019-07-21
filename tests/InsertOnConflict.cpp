#include <iostream>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include "MockDb.h"

#include "TabBar.h"
#include "TabFoo.h"

namespace sql = sqlpp::postgresql;

int InsertOnConflict(int argc, char** argv)
{
  model::TabFoo foo = {};
  //model::TabBar bar = {};

  //auto config = std::make_shared<sql::connection_config>();

  MockDb::_serializer_context_t printer = {};

  // Test on conflict
  auto insert1 = sql::insert_into(foo).default_values().on_conflict().do_nothing();
  //auto insert2 = sql::insert_into(foo).on_conflict().do_update();
  std::cout << serialize(insert1, printer).str() << std::endl;

  //sql::insert_into(foo).default_values().on_conflict().do_nothing();
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
