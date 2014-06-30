#ifndef SQLPP_POSTGRESQL_CONNECTION_HANDLE_H
#define SQLPP_POSTGRESQL_CONNECTION_HANDLE_H

#include <memory>
#include <postgresql/libpq-fe.h>

namespace sqlpp {

	namespace postgresql {

		// Forward declaration
		struct connection_config;

		namespace detail {

			struct connection_handle {
				const std::shared_ptr<connection_config> config;
				PGconn *postgres {nullptr};

				connection_handle(const std::shared_ptr<connection_config> &config);
				~connection_handle();
				connection_handle(const connection_handle &) = delete;
				connection_handle(connection_handle &&) = delete;
				connection_handle &operator=(const connection_handle &) = delete;
				connection_handle &operator=(connection_handle &&) = delete;
			};
		}
	}
}

#endif
