#ifndef SQLPP_POSTGRESQL_CONNECTION_CONFIG_H
#define SQLPP_POSTGRESQL_CONNECTION_CONFIG_H

#include <string>

namespace sqlpp {

	namespace postgresql {

		struct connection_config {
			enum class sslmode_t {
				disable,
				allow,
				prefer,
				require,
				verify_ca,
				verify_full
			};
			std::string host;
			std::string hostaddr;
			uint32_t port {5432};
			std::string dbname;
			std::string user;
			std::string password;
			uint32_t connect_timeout {0};
			std::string client_encoding;
			std::string options;
			std::string application_name;
			std::string fallback_application_name;
			bool keepalives {true};
			uint32_t keepalives_idle {0};
			uint32_t keepalives_interval {0};
			uint32_t keepalives_count {0};
			sslmode_t sslmode {sslmode_t::prefer};
			bool sslcompression {true};
			std::string sslcert;
			std::string sslkey;
			std::string sslrootcert;
			std::string sslcrl;
			std::string requirepeer;
			std::string krbsrvname;
			std::string service;
			//bool auto_reconnect {true};
			bool debug {false};

			bool operator==(const connection_config &other) {
				return (other.host == host
						&& other.hostaddr == hostaddr
						&& other.port == port
						&& other.dbname == dbname
						&& other.user == user
						&& other.password == password
						&& other.connect_timeout == connect_timeout
						&& other.client_encoding == client_encoding
						&& other.options == options
						&& other.application_name == application_name
						&& other.keepalives == keepalives
						&& other.keepalives_idle == keepalives_idle
						&& other.keepalives_interval == keepalives_interval
						&& other.keepalives_count == keepalives_count
						&& other.sslmode == sslmode
						&& other.sslcompression == sslcompression
						&& other.sslcert == sslcert
						&& other.sslkey == sslkey
						&& other.sslrootcert == sslrootcert
						&& other.sslcrl == sslcrl
						&& other.requirepeer == requirepeer
						&& other.krbsrvname == krbsrvname
						&& other.service == service
						&& other.debug == debug);
			}
			bool operator!=(const connection_config &other) {
				return !operator==(other);
			}
		};
	}
}

#endif
