#include "connection_handle.h"

#include <sqlpp11/postgresql/connection_config.h>
#include <sqlpp11/exception.h>

#include <iostream> // DEBUG

namespace sqlpp {

	namespace postgresql {

		namespace detail {

			connection_handle::connection_handle(const std::shared_ptr<connection_config> &conf) : config(conf) {

				if (config->debug) {
					std::cerr << "PostgreSQL debug: connecting to the database server." << std::endl;
				}

				// Open connection
				std::string conninfo = "user=";
				conninfo.append(config->username);
				conninfo.append(" password=" + config->password);
				conninfo.append(" dbname=" + config->database);
				conninfo.append(" hostaddr=" + config->host);
				this->postgres = PQconnectdb(conninfo.c_str());
				if (PQstatus(this->postgres) != CONNECTION_OK) {
					throw sqlpp::exception("PostgreSQL error: failed to connect to the database");
				}
			}

			connection_handle::~connection_handle() {

				// Debug
				if (config->debug) {
					std::cerr << "PostgreSQL debug: closing database connection." << std::endl;
				}

				// Close connection
				if (this->postgres) {
					PQfinish(this->postgres);
				}
			}
		}
	}
}

