#include "prepared_statement_handle.h"
#include <random>
#include <sqlpp11/postgresql/connection_config.h>

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

    namespace detail
    {
      statement_handle_t::statement_handle_t(connection_handle& _connection) : connection(_connection)
      {
      }

      statement_handle_t::~statement_handle_t()
      {
        clearResult();
      }

      bool statement_handle_t::operator!() const
      {
        return !valid;
      }

      void statement_handle_t::clearResult()
      {
        if (result)
        {
          result.clear();
        }
      }

      bool statement_handle_t::debug() const
      {
        return connection.config->debug;
      }

      prepared_statement_handle_t::prepared_statement_handle_t(connection_handle& _connection,
                                                               std::string stmt,
                                                               const size_t& paramCount)
          : statement_handle_t(_connection), nullValues(paramCount), paramValues(paramCount)
      {
        generate_name();
        prepare(std::move(stmt));
      }

      prepared_statement_handle_t::~prepared_statement_handle_t()
      {
        if (valid && !_name.empty())
        {
          connection.deallocate_prepared_statement(_name);
        }
      }

      void prepared_statement_handle_t::execute()
      {
        int size = static_cast<int>(paramValues.size());

        std::vector<const char*> values;
        for (int i = 0; i < size; i++)
          values.push_back(nullValues[i] ? nullptr : const_cast<char*>(paramValues[i].c_str()));

        // Execute prepared statement with the parameters.
        clearResult();
        valid = false;
        count = 0;
        totalCount = 0;
        result = PQexecPrepared(connection.postgres, _name.data(), size, values.data(), nullptr, nullptr, 0);
		/// @todo validate result? is it really valid
        valid = true;
      }

      void prepared_statement_handle_t::generate_name()
      {
        // Generate a random name for the prepared statement
        while (connection.prepared_statement_names.find(_name) != connection.prepared_statement_names.end())
        {
          std::generate_n(_name.begin(), 6, []() {
            constexpr static auto charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            "abcdefghijklmnopqrstuvwxyz";
            constexpr size_t max = (sizeof(charset) - 1);
            std::random_device rd;
            return charset[rd() % max];
          });
        }
        connection.prepared_statement_names.insert(_name);
      }

      void prepared_statement_handle_t::prepare(std::string stmt)
      {
        // Create the prepared statement
        result = PQprepare(connection.postgres, _name.c_str(), stmt.c_str(), 0, nullptr);
        valid = true;
      }
    }
  }
}
