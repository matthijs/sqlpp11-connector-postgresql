#ifndef SQLPP_POSTGRESQL_PREPARED_STATEMENT_HANDLE_H
#define SQLPP_POSTGRESQL_PREPARED_STATEMENT_HANDLE_H

#include <postgresql/libpq-fe.h>

namespace sqlpp {

	namespace postgresql {

		namespace detail {

			struct prepared_statement_handle_t {
				PGresult *stmt {nullptr};
				PGresult *result {nullptr};
				std::string name {"prepare"};
				bool debug {false};
				uint32_t count {0};
				uint32_t totalCount = {0};
				uint32_t fields = {0};

				// ctor
				prepared_statement_handle_t(PGresult *_stmt, bool _debug) : stmt(_stmt), debug(_debug) {}
				prepared_statement_handle_t(const prepared_statement_handle_t &) = delete;
				prepared_statement_handle_t(prepared_statement_handle_t &&) = default;
				prepared_statement_handle_t &operator=(const prepared_statement_handle_t &) = delete;
				prepared_statement_handle_t &operator=(prepared_statement_handle_t &&) = default;

				~prepared_statement_handle_t() {
					if (stmt) {

						// Execute DEALLOCATE on the connection_handle for this
						// prepared statement.

						// Now clear the result
						PQclear(stmt);
					}
					if (result) {
						PQclear(result);
					}
				}

				bool operator!() const {
					return !stmt;
				}
			};
		}
	}
}

#endif
