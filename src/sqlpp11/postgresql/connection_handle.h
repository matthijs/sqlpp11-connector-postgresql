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

#ifndef SQLPP_POSTGRESQL_CONNECTION_HANDLE_H
#define SQLPP_POSTGRESQL_CONNECTION_HANDLE_H

#include <memory>
#include <set>

#include <libpq-fe.h>
#include <sqlpp11/postgresql/visibility.h>

namespace sqlpp
{
  namespace postgresql
  {
    // Forward declaration
    struct connection_config;

    namespace detail
    {
      struct DLL_LOCAL connection_handle
      {
        const std::shared_ptr<connection_config> config;
        PGconn* postgres{nullptr};
        std::set<std::string> prepared_statement_names;

        connection_handle(const std::shared_ptr<connection_config>& config);
        ~connection_handle();
        connection_handle(const connection_handle&) = delete;
        connection_handle(connection_handle&&) = delete;
        connection_handle& operator=(const connection_handle&) = delete;
        connection_handle& operator=(connection_handle&&) = delete;

        PGconn* native() const
        {
          return postgres;
        }

        void deallocate_prepared_statement(const std::string& name)
        {
          std::string cmd = "DEALLOCATE \"" + name + "\"";
          PGresult* result = PQexec(postgres, cmd.c_str());
          PQclear(result);
          prepared_statement_names.erase(name);
        }
      };
    }
  }
}

#endif
