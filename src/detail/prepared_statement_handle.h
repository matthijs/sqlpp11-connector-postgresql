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

#ifndef SQLPP_POSTGRESQL_PREPARED_STATEMENT_HANDLE_H
#define SQLPP_POSTGRESQL_PREPARED_STATEMENT_HANDLE_H

#include <iostream>
#include <string>
#include <vector>

#include <postgresql/libpq-fe.h>
#include <sqlpp11/postgresql/result.h>

namespace sqlpp
{
  namespace postgresql
  {
    namespace detail
    {
      struct statement_handle_t
      {
        PGconn* connection{nullptr};
        Result result;
        bool valid{false};
        bool debug{false};
        uint32_t count{0};
        uint32_t totalCount = {0};
        uint32_t fields = {0};

        // ctor
        statement_handle_t(PGconn* _connection, bool _debug) : connection(_connection), debug(_debug)
        {
        }
        statement_handle_t(const statement_handle_t&) = delete;
        statement_handle_t(statement_handle_t&&) = default;
        statement_handle_t& operator=(const statement_handle_t&) = delete;
        statement_handle_t& operator=(statement_handle_t&&) = default;

        virtual ~statement_handle_t()
        {
          clearResult();
        }

        bool operator!() const
        {
          return !valid;
        }

        void clearResult()
        {
          if (result)
          {
            result.clear();
          }
        }
      };

      struct prepared_statement_handle_t : public statement_handle_t
      {
        std::string name{"xxxxxx"};

        // Store prepared statement arguments
        std::vector<bool> nullValues;
        std::vector<std::string> paramValues;

        // ctor
        prepared_statement_handle_t(PGconn* _connection, const size_t& paramCount, bool _debug)
            : statement_handle_t(_connection, _debug), nullValues(paramCount), paramValues(paramCount)
        {
        }
        prepared_statement_handle_t(const prepared_statement_handle_t&) = delete;
        prepared_statement_handle_t(prepared_statement_handle_t&&) = default;
        prepared_statement_handle_t& operator=(const prepared_statement_handle_t&) = delete;
        prepared_statement_handle_t& operator=(prepared_statement_handle_t&&) = default;

        ~prepared_statement_handle_t()
        {
          clearResult();

          // Execute DEALLOCATE on the connection_handle for this
          // prepared statement.
          if (valid && !name.empty())
          {
            std::string cmd = "DEALLOCATE \"" + name + "\"";
            PGresult* result = PQexec(connection, cmd.c_str());
            PQclear(result);
          }

          // TODO: remove the name from the prepared_statement_names
          // in the connection_handle.
        }
      };
    }
  }
}

#endif
