/**
 * Copyright © 2014-2015, Matthijs Möhlmann
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

#ifndef SQLPP_POSTGRESQL_RETURNING_H
#define SQLPP_POSTGRESQL_RETURNING_H

#include <sqlpp11/statement.h>

namespace sqlpp
{
  namespace postgresql
  {
    // RETURNING DATA
    template <typename Database, typename... Expressions>
    struct returning_data_t
    {
    };

    // RETURNING(EXPR)
    template <typename Database, typename... Expressions>
    struct returning_t
    {
    };

    struct no_returning_t
    {
      template <typename Policies>
      struct _base_t  // base class for the statement
      {
        template <typename Check, typename T>
        using _new_statement_t = new_statement_t<Check::value, Policies, no_returning_t, T>;

        template <typename Database, typename... Expressions>
        auto returning(Expressions... expressions) const
            -> _new_statement_t<std::true_type, returning_t<void, Expressions...>>
        {
          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  returning_data_t<Database, Expressions...>{expressions...}};
        }
      };
    };
  }

  // Interpreter
  template <typename Context, typename Database, typename... Expressions>
  struct serializer_t<Context, postgresql::returning_data_t<Database, Expressions...>>
  {
    using _serialize_check = consistent_t;
    using T = postgresql::returning_data_t<Database, Expressions...>;

    static Context& _(const T&, Context& context)
    {
      context << " RETURNING ";

      return context;
    }
  };
}

#endif
