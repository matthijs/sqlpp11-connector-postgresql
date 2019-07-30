/**
 * Copyright © 2014-2019, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
  // model::TabBar bar = {};

  // auto config = std::make_shared<sql::connection_config>();

  MockDb::_serializer_context_t printer = {};

  // Test on conflict
  auto insert1 = sql::insert_into(foo).default_values().on_conflict().do_nothing();
  std::cout << serialize(insert1, printer).str() << std::endl;
  printer.reset();

  auto insert2 = sql::insert_into(foo).default_values().on_conflict().do_update(foo.beta = 5, foo.gamma = "test bla", foo.c_bool = true);
  std::cout << serialize(insert2, printer).str() << std::endl;
  printer.reset();

  // With where statement
  auto insert3 = sql::insert_into(foo).default_values().on_conflict().do_update(foo.beta = 5, foo.gamma = "test bla", foo.c_bool = true).where(foo.beta == 2);
  std::cout << serialize(insert3, printer).str() << std::endl;
  printer.reset();

  // Returning
  auto insert4 = sql::insert_into(foo).default_values().on_conflict().do_update(foo.beta = 5, foo.gamma = "test bla", foo.c_bool = true).returning(foo.beta);
  std::cout << serialize(insert4, printer).str() << std::endl;
  printer.reset();

  return 0;
}
