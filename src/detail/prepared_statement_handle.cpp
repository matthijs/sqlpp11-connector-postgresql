#include "prepared_statement_handle.h"
#include <random>
#include <sqlpp11/postgresql/connection_config.h>

namespace sqlpp
{
  namespace postgresql
  {
    namespace detail
    {
      statement_handle_t::statement_handle_t(connection_handle& _connection, bool _debug)
          : connection(_connection), debug(_debug)
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

      prepared_statement_handle_t::prepared_statement_handle_t(connection_handle& _connection, const size_t& paramCount)
          : statement_handle_t(_connection, _connection.config->debug), nullValues(paramCount), paramValues(paramCount)
      {
        generate_name();
      }

      prepared_statement_handle_t::~prepared_statement_handle_t()
      {
        if (valid && !name.empty())
        {
          connection.deallocate_prepared_statement(name);
        }
      }

      void prepared_statement_handle_t::generate_name()
      {
        // Generate a random name for the prepared statement
        while (connection.prepared_statement_names.find(name) != connection.prepared_statement_names.end())
        {
          std::generate_n(name.begin(), 6, []() {
            constexpr static auto charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            "abcdefghijklmnopqrstuvwxyz";
            constexpr size_t max = (sizeof(charset) - 1);
            std::random_device rd;
            return charset[rd() % max];
          });
        }
        connection.prepared_statement_names.insert(name);
      }
    }
  }
}
