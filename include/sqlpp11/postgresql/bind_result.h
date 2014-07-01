#ifndef SQLPP_POSTGRESQL_BIND_RESULT_H
#define SQLPP_POSTGRESQL_BIND_RESULT_H

#include <memory>

namespace sqlpp {

	namespace postgresql {

		namespace detail {
			struct prepared_statement_handle_t;
		}

		class bind_result_t {

			private:
				std::shared_ptr<detail::prepared_statement_handle_t> _handle;

				bool next_impl();

			public:
				bind_result_t() = default;
				bind_result_t(const std::shared_ptr<detail::prepared_statement_handle_t> &handle);
				bind_result_t(const bind_result_t &) = delete;
				bind_result_t(bind_result_t &&) = default;
				bind_result_t &operator=(const bind_result_t &) = delete;
				bind_result_t &operator=(bind_result_t &&) = default;
				~bind_result_t() = default;

				bool operator==(const bind_result_t &rhs) const {
					return (this->_handle == rhs._handle);
				}

				template<typename ResultRow>
					void next(ResultRow &result_row) {
						if (!this->_handle) {
							result_row._invalidate();
							return;
						}

						if (this->next_impl()) {
							if (!result_row) {
								result_row._validate();
							}
							result_row._bind(*this);
						} else {
							if (result_row) {
								result_row._invalidate();
							}
						}
					}

				void _bind_boolean_result(size_t index, signed char *value, bool *is_null);
				void _bind_floating_point_result(size_t index, double *value, bool *is_null);
				void _bind_integral_result(size_t index, int64_t *value, bool *is_null);
				void _bind_text_result(size_t index, const char **value, size_t *len);
		};
	}
}

#endif
