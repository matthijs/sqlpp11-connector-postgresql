#ifndef SQLPP_POSTGRESQL_CONNECTION_CONFIG_H
#define SQLPP_POSTGRESQL_CONNECTION_CONFIG_H

#include <string>

namespace sqlpp {

	namespace postgresql {

		struct connection_config {
			std::string host;
			std::string username;
			std::string password;
			std::string database;
			uint32_t port {5432};
			std::string unix_socket;
			bool auto_reconnect {true};
			bool debug {false};

			bool operator==(const connection_config &other) {
				return (other.host == host
						&& other.username == username
						&& other.password == password
						&& other.database == database
						&& other.port == port
						&& other.unix_socket == unix_socket
						&& other.auto_reconnect == auto_reconnect
						&& other.debug == debug);
			}
			bool operator!=(const connection_config &other) {
				return !operator==(other);
			}
		};
	}
}

#endif
