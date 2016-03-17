#include <iostream>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>

#include "MockDb.h"
#include "TabFoo.h"
#include "TabBar.h"

namespace sql = sqlpp::postgresql;
int Returning(int argc, char **argv) {
    model::TabFoo tab;
    model::TabBar bar;
//    auto config = std::make_shared<sql::connection_config>();
//    config->user = "postgres";
//    config->password="postgres";
//    config->dbname = "test";
//    config->host = "localhost";
//    config->port = 5432;
//    config->debug = true;
//    try
//    {
//        sql::connection db(config);
//    }
//    catch (const sql::pg_exception &){
//        std::cerr << "For testing, you'll need to create a database sqlpp_postgresql" << std::endl;
//        throw;
//    }
//    catch (const sqlpp::exception&)
//    {
//        std::cerr << "For testing, you'll need to create a database sqlpp_postgresql" << std::endl;
//        throw;
//    }

//    sql::connection db(config);

//    db.execute(R"(DROP TABLE IF EXISTS tabfoo;)");
//    db.execute(R"(CREATE TABLE tabfoo
//               (
//                 alpha bigserial NOT NULL,
//                 beta smallint,
//                 gamma text,
//                 c_bool boolean,
//                 c_timepoint timestamp with time zone,
//                 c_day date
//               ))");
//    const auto &timestamp = db(sqlpp::postgresql::insert_into(tab).set(tab.gamma = "s").returning(tab.c_timepoint));


    MockDb db = {};
    MockDb::_serializer_context_t printer = {};
    std::cout << serialize(sqlpp::postgresql::insert_into(tab)
                           .set(tab.gamma = "s")
                           .returning(tab.c_timepoint), printer).str() << std::endl;
    printer.reset();

    // must fail!
    std::cout << serialize(sqlpp::postgresql::insert_into(tab)
                           .set(tab.gamma = "s")
                           .returning(bar.c_int), printer).str() << std::endl;
    printer.reset();



    //  std::cout << serialize(sqlpp::postgresql::insert_into(t_bar)
    //                         .set(t_bar.gamma = true, t_bar.beta = "test")
    //                         .returning(t_foo.delta), printer).str() << std::endl;

    return 0;
}
