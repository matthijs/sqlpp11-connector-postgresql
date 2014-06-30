#ifndef SQLPP_POSTGRESQL_PREPARED_STATEMENT_H
#define SQLPP_POSTGRESQL_PREPARED_STATEMENT_H

#include <memory>

namespace sqlpp {

	namespace postgresql {

		// Forward declaration
		struct connection;

		// Detail namespace
		namespace detail {

			struct prepared_statement_handle_t;
		}

		class prepared_statement_t {
			friend sqlpp::postgresql::connection;
			private:
				std::shared_ptr<detail::prepared_statement_handle_t> _handle;

			public:
				prepared_statement_t() = delete;
				prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t> &&handle);
				prepared_statement_t(const prepared_statement_t &) = delete;
				prepared_statement_t(prepared_statement_t &&) = default;
				prepared_statement_t &operator=(const prepared_statement_t &) = delete;
				prepared_statement_t &operator=(prepared_statement_t &&) = default;
				~prepared_statement_t() = default;

				bool operator==(const prepared_statement_t &rhs) {
					return (this->_handle == rhs._handle);
				}

				void _bind_boolean_parameter(size_t index, const signed char *value, bool is_null);
				void _bind_floating_point_parameter(size_t index, const signed char *value, bool is_null);
				void _bind_integral_parameter(size_t index, const signed char *value, bool is_null);
				void _bind_text_parameter(size_t index, const signed char *value, bool is_null);
		};
	}
}

#endif
