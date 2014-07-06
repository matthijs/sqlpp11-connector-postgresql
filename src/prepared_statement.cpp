#include <sqlpp11/postgresql/prepared_statement.h>
#include <sqlpp11/exception.h>

#include "detail/prepared_statement_handle.h"

#include <iostream>
#include <sstream>

namespace sqlpp {

	namespace postgresql {

		// ctor
		prepared_statement_t::prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t> &&handle) : _handle{std::move(handle)} {
			if (_handle && _handle->debug) {
				std::cerr << "PostgreSQL debug: constructing prepared_statement, using handle at: " << _handle.get() << std::endl;
			}
		}

		void prepared_statement_t::_bind_boolean_parameter(size_t index, const signed char *value, bool is_null) {
			if (_handle->debug) {
				std::cerr << "PostgreSQL debug: binding boolean parameter " << (*value ? "true" :  "false") << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
			}

			_handle->nullValues[index] = is_null;
			if (!is_null) {
				if (*value) {
					_handle->paramValues[index] = "TRUE";
				} else {
					_handle->paramValues[index] = "FALSE";
				}
			}
		}

		void prepared_statement_t::_bind_floating_point_parameter(size_t index, const double *value, bool is_null) {
			if (_handle->debug) {
				std::cerr << "PostgreSQL debug: binding floating_point parameter " << *value << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
			}

			_handle->nullValues[index] = is_null;
			if (!is_null) {
				_handle->paramValues[index] = std::to_string(*value);
			}
		}

		void prepared_statement_t::_bind_integral_parameter(size_t index, const int64_t *value, bool is_null) {
			if (_handle->debug) {
				std::cerr << "PostgreSQL debug: binding integral parameter " << *value << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
			}

			// Assign values
			_handle->nullValues[index] = is_null;
			if (!is_null) {
				_handle->paramValues[index] = std::to_string(*value);
			}
		}

		void prepared_statement_t::_bind_text_parameter(size_t index, const std::string *value, bool is_null) {
			if (_handle->debug) {
				std::cerr << "PostgreSQL debug: binding text parameter " << *value << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
			}

			// Assign values
			_handle->nullValues[index] = is_null;
			if (!is_null) {
				_handle->paramValues[index] = *value;
			}
		}
	}
}

