#ifndef SQLPP_POSTGRESQL_PREPARED_STATEMENT_HANDLE_H
#define SQLPP_POSTGRESQL_PREPARED_STATEMENT_HANDLE_H

#include <iostream>
#include <postgresql/libpq-fe.h>

namespace sqlpp {

	namespace postgresql {

		namespace detail {

			struct prepared_statement_handle_t {
				PGconn *connection {nullptr};
				PGresult *result {nullptr};
				std::string name {"xxxxxx"};
				bool valid {false};
				bool debug {false};
				uint32_t count {0};
				uint32_t totalCount = {0};
				uint32_t fields = {0};

				// ctor
				prepared_statement_handle_t(PGconn *_connection, bool _debug) : connection(_connection), debug(_debug) {}
				prepared_statement_handle_t(const prepared_statement_handle_t &) = delete;
				prepared_statement_handle_t(prepared_statement_handle_t &&) = default;
				prepared_statement_handle_t &operator=(const prepared_statement_handle_t &) = delete;
				prepared_statement_handle_t &operator=(prepared_statement_handle_t &&) = default;

				~prepared_statement_handle_t() {

					// Clear the result
					if (result) {
						PQclear(result);
					}

					// Execute DEALLOCATE on the connection_handle for this
					// prepared statement.
					if (valid && !name.empty()) {
						std::string cmd = "DEALLOCATE \"" + name + "\"";
						PGresult *result = PQexec(connection, cmd.c_str());
						PQclear(result);
					}

					// TODO: remove the name from the prepared_statement_names
					// in the connection_handle.
				}

				bool operator!() const {
					return !valid;
				}
			};
		}
	}
}

#endif
