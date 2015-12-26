#include <iostream>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/postgresql/insert.h>
//#include <sqlpp11/sqlpp11.h>

#include "Sample.h"

int Returning(int argc, char **argv) {

  // Configuration
  //auto config = std::make_shared<sqlpp::postgresql::connection_config>();

  // DB connection
  //sqlpp::postgresql::connection db(config);

  // Model
  test::TabBar t_bar;
  auto ins_news = insert_into(t_bar)
	  .set(t_bar.gamma = true, t_bar.beta = "test")
	  .returning(t_bar.beta);

  return 0;
}
