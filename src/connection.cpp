/**
 * Copyright © 2014-2016, Matthijs Möhlmann
 * Copyright © 2015-2016, Bartosz Wieczorek
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

#include <sqlpp11/exception.h>
#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/postgresql/exception.h>
#include <sqlpp11/transaction.h>

#include <algorithm>
#include <iostream>

#if __cplusplus == 201103L
#include "make_unique.h"
#endif

#include "detail/connection_handle.h"
#include "detail/prepared_statement_handle.h"

#ifdef SQLPP_DYNAMIC_LOADING
#include <sqlpp11/postgresql/dynamic_libpq.h>
#endif

namespace sqlpp
{
  namespace postgresql
  {
#ifdef SQLPP_DYNAMIC_LOADING
    using namespace dynamic;
#endif

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

        auto prepared_statement = std::make_unique<detail::prepared_statement_handle_t>(handle, paramCount);

        // Create the prepared statement
        prepared_statement->result =
            PQprepare(handle.postgres, prepared_statement->name.c_str(), stmt.c_str(), 0, nullptr);
        prepared_statement->valid = true;
        return prepared_statement;
      }

      void execute_prepared_statement(detail::connection_handle& handle, detail::prepared_statement_handle_t& prepared)
      {
        int size = static_cast<int>(prepared.paramValues.size());
        // Execute a prepared statement
        std::vector<char*> paramValues(size);
        // int paramLengths[prepared.paramValues.size()];
        for (int i = 0; i < size; i++)
          paramValues[i] = prepared.nullValues[i] ? nullptr : const_cast<char*>(prepared.paramValues[i].c_str());

        // Execute prepared statement with the parameters.
        prepared.clearResult();
        prepared.valid = false;
        prepared.count = 0;
        prepared.totalCount = 0;
        prepared.result =
            PQexecPrepared(handle.postgres, prepared.name.c_str(), size, paramValues.data(), nullptr, nullptr, 0);

        prepared.valid = true;
      }
    }

    connection::connection(const std::shared_ptr<connection_config>& config)
        : _handle(new detail::connection_handle(config))
    {
    }

    connection::~connection()
    {
    }

    connection::connection(connection&& other)
    {
      this->_transaction_active = other._transaction_active;
      this->_handle = std::move(other._handle);
    }

    connection& connection::operator=(connection&& other)
    {
      if (this != &other)
      {
        // TODO: check this logic
        this->_transaction_active = other._transaction_active;
        this->_handle = std::move(other._handle);
      }
      return *this;
    }

    std::shared_ptr<detail::statement_handle_t> connection::execute(const std::string& stmt)
    {
      validate_connection_handle();
      if (_handle->config->debug)
      {
        std::cerr << "PostgreSQL debug: executing: " << stmt << std::endl;
      }

      auto result = std::make_shared<detail::statement_handle_t>(*_handle, _handle->config->debug);
      result->result = PQexec(_handle->native(), stmt.c_str());
      result->valid = true;

      return result;
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
      validate_connection_handle();
      return {prepare_statement(*_handle, stmt, paramCount)};
    }

    bind_result_t connection::run_prepared_select_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return {prep._handle};
    }

    size_t connection::run_prepared_execute_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    size_t connection::run_prepared_insert_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    size_t connection::run_prepared_update_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    size_t connection::run_prepared_remove_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    void connection::set_default_isolation_level(isolation_level level)
    {
      std::string level_str = "read uncommmitted";
      switch (level)
      {
        /// @todo what about undefined ?
        case isolation_level::read_committed:
          level_str = "read committed";
          break;
        case isolation_level::read_uncommitted:
          level_str = "read uncommitted";
          break;
        case isolation_level::repeatable_read:
          level_str = "repeatable read";
          break;
        case isolation_level::serializable:
          level_str = "serializable";
          break;
        default:
          throw sqlpp::exception("Invalid isolation level");
      }
      std::string cmd = "SET default_transaction_isolation to '" + level_str + "'";
      execute(cmd);
    }

    isolation_level connection::get_default_isolation_level()
    {
      /// @todo run execute
      PGresult* res = PQexec(_handle->postgres, "SHOW default_transaction_isolation;");
      auto status = PQresultStatus(res);
      if ((status != PGRES_TUPLES_OK) && (status != PGRES_COMMAND_OK))
      {
        PQclear(res);
        throw sqlpp::exception("PostgreSQL error: could not read default_transaction_isolation");
      }

      std::string in{PQgetvalue(res, 0, 0)};
      PQclear(res);
      if (in == "read committed")
      {
        return isolation_level::read_committed;
      }
      else if (in == "read uncommitted")
      {
        return isolation_level::read_uncommitted;
      }
      else if (in == "repeatable read")
      {
        return isolation_level::repeatable_read;
      }
      else if (in == "serializable")
      {
        return isolation_level::serializable;
      }
      return isolation_level::undefined;
    }

    // TODO: Fix escaping.
    std::string connection::escape(const std::string& s) const
    {
      validate_connection_handle();
      // Escape strings
      std::string result;
      result.resize((s.size() * 2) + 1);

      int err;
      size_t length = PQescapeStringConn(_handle->postgres, &result[0], s.c_str(), s.size(), &err);
      result.resize(length);
      return result;
    }

    //! start transaction
    void connection::start_transaction(sqlpp::isolation_level level)
    {
      if (_transaction_active)
      {
        throw sqlpp::exception("PostgreSQL error: transaction already open");
      }
      switch (level)
      {
        case isolation_level::serializable:
        {
          execute("BEGIN ISOLATION LEVEL SERIALIZABLE");
          break;
        }
        case isolation_level::repeatable_read:
        {
          execute("BEGIN ISOLATION LEVEL REPEATABLE READ");
          break;
        }
        case isolation_level::read_committed:
        {
          execute("BEGIN ISOLATION LEVEL READ COMMITTED");
          break;
        }
        case isolation_level::read_uncommitted:
        {
          execute("BEGIN ISOLATION LEVEL READ UNCOMMITTED");
          break;
        }
        case isolation_level::undefined:
        {
          execute("BEGIN");
          break;
        }
      }
      _transaction_active = true;
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

    //! commit transaction (or throw transaction if transaction has finished already)
    void connection::commit_transaction()
    {
      if (!_transaction_active)
      {
        throw sqlpp::exception("PostgreSQL error: transaction failed or finished.");
      }

      _transaction_active = false;
      execute("COMMIT");
    }

    //! rollback transaction
    void connection::rollback_transaction(bool report)
    {
      if (!_transaction_active)
      {
        throw sqlpp::exception("PostgreSQL error: transaction failed or finished.");
      }
      execute("ROLLBACK");
      if (report)
      {
        std::cerr << "PostgreSQL warning: rolling back unfinished transaction" << std::endl;
      }

      _transaction_active = false;
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
  }
}
