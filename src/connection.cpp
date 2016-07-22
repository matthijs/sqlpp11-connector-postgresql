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

#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/postgresql/exception.h>

#include <algorithm>
#include <iostream>
#include <random>

#include "detail/prepared_statement_handle.h"
#include "detail/connection_handle.h"

namespace sqlpp
{
  namespace postgresql
  {
    namespace
    {
      std::unique_ptr<detail::prepared_statement_handle_t> prepare_statement(detail::connection_handle& handle,
                                                            const std::string& stmt,
                                                            const size_t& paramCount)
      {
        if (handle.config->debug)
        {
          std::cerr << "PostgreSQL debug: preparing: " << stmt << std::endl;
        }

        auto result = std::make_unique<detail::prepared_statement_handle_t>(handle.postgres, paramCount, handle.config->debug);

        // Generate a random name for the prepared statement
        while (std::find(handle.prepared_statement_names.begin(), handle.prepared_statement_names.end(), result->name) !=
               handle.prepared_statement_names.end())
        {
          std::generate_n(result->name.begin(), 6, []()
                          {
                            const char charset[] = "0123456789"
                                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                   "abcdefghijklmnopqrstuvwxyz";
                            constexpr size_t max = (sizeof(charset) - 1);
                            std::random_device rd;
                            return charset[rd() % max];
                          });
        }
        handle.prepared_statement_names.push_back(result->name);

        // Create the prepared statement
        result->result = PQprepare(handle.postgres, result->name.c_str(), stmt.c_str(), 0, nullptr);

        result->valid = true;
        return std::move(result);
      }

      void execute_prepared_statement(detail::connection_handle& handle, detail::prepared_statement_handle_t& prepared)
      {
        // Execute a prepared statement
        char* paramValues[prepared.paramValues.size()];
        // int paramLengths[prepared.paramValues.size()];
        for (uint32_t i = 0; i < prepared.paramValues.size(); i++)
        {
          if (!prepared.nullValues[i])
          {
            paramValues[i] = const_cast<char*>(prepared.paramValues[i].c_str());
            // paramLengths[i] = prepared.paramValues[i].size();
          }
          else
          {
            paramValues[i] = nullptr;
            // paramLengths[i] = 0;
          }
        }

        // Execute prepared statement with the parameters.

        prepared.clearResult();
        prepared.valid = false;
        prepared.count = 0;
        prepared.totalCount = 0;
        prepared.result = PQexecPrepared(handle.postgres, prepared.name.c_str(), prepared.paramValues.size(),
                                         paramValues, nullptr, nullptr, 0);

        prepared.valid = true;
      }
    }

    std::shared_ptr<detail::statement_handle_t> connection::execute(const std::string& stmt)
    {
      if (_handle->config->debug)
      {
        std::cerr << "PostgreSQL debug: executing: " << stmt << std::endl;
      }

      auto result = std::make_shared<detail::statement_handle_t>(native_handle(), _handle->config->debug);
      result->result = PQexec(native_handle(), stmt.c_str());
      result->valid = true;

      return result;
    }

    connection::connection(const std::shared_ptr<connection_config>& config)
        : _handle(new detail::connection_handle(config))
    {
    }

    connection::~connection()
    {
    }

    // direct execution
    bind_result_t connection::select_impl(const std::string& stmt)
    {
      return execute(stmt);
    }

    size_t connection::insert_impl(const std::string& stmt)
    {
      return execute(stmt)->result.affected_rows();
    }

    size_t connection::update_impl(const std::string& stmt)
    {
      return execute(stmt)->result.affected_rows();
    }

    size_t connection::remove_impl(const std::string& stmt)
    {
      return execute(stmt)->result.affected_rows();
    }

    // prepared execution
    prepared_statement_t connection::prepare_impl(const std::string& stmt, const size_t& paramCount)
    {
      return {prepare_statement(*_handle, stmt, paramCount)};
    }

    bind_result_t connection::run_prepared_select_impl(prepared_statement_t& prep)
    {
      execute_prepared_statement(*_handle, *prep._handle.get());
      return {prep._handle};
    }

    size_t connection::run_prepared_execute_impl(prepared_statement_t& prep)
    {
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    size_t connection::run_prepared_insert_impl(prepared_statement_t& prep)
    {
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    size_t connection::run_prepared_update_impl(prepared_statement_t& prep)
    {
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    size_t connection::run_prepared_remove_impl(prepared_statement_t& prep)
    {
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    std::string connection::escape(const std::string& s) const
    {
      // Escape strings
      std::string result;
      result.resize((s.size() * 2) + 1);

      int err;
      size_t length = PQescapeStringConn(_handle->postgres, &result[0], s.c_str(), s.size(), &err);
      result.resize(length);
      return result;
    }

    //! start transaction
    void connection::start_transaction()
    {
      execute("BEGIN");
    }

    //! create savepoint
    void connection::savepoint(const std::string& name)
    {
      /// NOTE prevent from sql injection?
      execute("SAVEPOINT " + name);
    }

    //! ROLLBACK TO SAVEPOINT
    void connection::rollback_to_savepoint(const std::string& name)
    {
      /// NOTE prevent from sql injection?
      execute("ROLLBACK TO SAVEPOINT " + name);
    }

    //! release_savepoint
    void connection::release_savepoint(const std::string& name)
    {
      /// NOTE prevent from sql injection?
      execute("RELEASE SAVEPOINT " + name);
    }

    //! commit transaction
    void connection::commit_transaction()
    {
      execute("COMMIT");
    }

    //! rollback transaction
    void connection::rollback_transaction(bool report)
    {
      execute("ROLLBACK");
      if (report)
      {
        std::cerr << "PostgreSQL warning: rolling back unfinished transaction" << std::endl;
      }
    }

    //! report rollback failure
    void connection::report_rollback_failure(const std::string& message) noexcept
    {
      std::cerr << "PostgreSQL error: " << message << std::endl;
    }

    uint64_t connection::last_insert_id(const std::string& table, const std::string& fieldname)
    {
      std::string sql = "SELECT currval('" + table + "_" + fieldname + "_seq')";
      PGresult* res = PQexec(_handle->postgres, sql.c_str());

      // Parse the number and return.
      std::string in{PQgetvalue(res, 0, 0)};
      PQclear(res);
      return std::stoi(in);
    }

    ::PGconn* connection::native_handle()
    {
      return _handle->postgres;
    }
  }
}
