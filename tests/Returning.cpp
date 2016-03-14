#include <iostream>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/postgresql/insert.h>
#include <sqlpp11/sqlpp11.h>

#include "MockDb.h"
#include "TabFoo.h"
#include "TabBar.h"

int Returning(int argc, char **argv) {

  // Configuration
  //auto config = std::make_shared<sqlpp::postgresql::connection_config>();

  // DB connection
  //sqlpp::postgresql::connection db(config);

  // Mock DB
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};

  // Model
//  model::TabBar t_bar;
//  model::TabFoo t_foo;
//  std::cout << serialize(sqlpp::postgresql::insert_into(t_bar)
//	  .set(t_bar.gamma = true, t_bar.beta = "test")
//	  .returning(t_bar.beta, t_bar.gamma), printer).str() << std::endl;
//  printer.reset();

//  std::cout << serialize(sqlpp::postgresql::insert_into(t_bar)
//	  .set(t_bar.gamma = true, t_bar.beta = "test")
//	  .returning(t_bar.beta), printer).str() << std::endl;
//  printer.reset();

//  std::cout << serialize(sqlpp::postgresql::insert_into(t_bar)
//                         .set(t_bar.gamma = true, t_bar.beta = "test")
//                         .returning(t_bar.beta), printer).str() << std::endl;
//  printer.reset();



//  std::cout << serialize(sqlpp::postgresql::insert_into(t_bar)
//                         .set(t_bar.gamma = true, t_bar.beta = "test")
//                         .returning(t_foo.delta), printer).str() << std::endl;

  return 0;
}
