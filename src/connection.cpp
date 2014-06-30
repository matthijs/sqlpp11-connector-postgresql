#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/exception.h>

#include <iostream>

#include "detail/prepared_statement_handle.h"
#include "detail/connection_handle.h"

namespace sqlpp {

	namespace postgresql {

		namespace {
			detail::prepared_statement_handle_t prepare_statement(detail::connection_handle &handle, const std::string &stmt) {
				if (handle.config->debug) {
					std::cerr << "PostgreSQL debug: preparing: " << stmt << std::endl;
				}

				detail::prepared_statement_handle_t result(nullptr, handle.config->debug);
				result.stmt = PQprepare(handle.postgres,
						"",
						stmt.c_str(),
						0,
						nullptr);
				std::string errmsg = "PostgreSQL error: ";
				ExecStatusType ret = PQresultStatus(result.stmt);
				switch(ret) {
					case PGRES_EMPTY_QUERY:
					case PGRES_COPY_OUT:
					case PGRES_COPY_IN:
					case PGRES_BAD_RESPONSE:
					case PGRES_NONFATAL_ERROR:
					case PGRES_FATAL_ERROR:
					case PGRES_COPY_BOTH:
						errmsg.append(PQresStatus(ret));
						throw sqlpp::exception(errmsg);
					case PGRES_COMMAND_OK:
					case PGRES_TUPLES_OK:
					case PGRES_SINGLE_TUPLE:
					default:
						break;
				}

				return result;
			}

			void execute_statement(detail::connection_handle &handle, detail::prepared_statement_handle_t &prepared) {

				// Execute a prepared statement
				prepared.result = PQexecPrepared(handle.postgres,
						prepared.name.c_str(),
						prepared.count,
						NULL,
						NULL,
						NULL,
						0);
			}
		}

		connection::connection(const std::shared_ptr<connection_config> &config) : _handle(new detail::connection_handle(config)) {
		}

		connection::~connection() {
		}

		// direct execution
		bind_result_t connection::select_impl(const std::string &stmt) {

			// Prepare statement
			std::unique_ptr<detail::prepared_statement_handle_t> prepared(new detail::prepared_statement_handle_t(prepare_statement(*_handle, stmt)));
			if (!prepared) {

				throw sqlpp::exception("PostgreSQL error: could not store result set");
			}

			// Execute statement
			execute_statement(*_handle, *prepared);
			return {std::move(prepared)};
		}

		size_t connection::insert_impl(const std::string &stmt) {

			// Prepare statement
			detail::prepared_statement_handle_t prepared = prepare_statement(*_handle, stmt);
			if (!prepared) {

				throw sqlpp::exception("PostgreSQL error: could not store result set");
			}

			// Execute statement
			execute_statement(*_handle, prepared);
			std::istringstream in(PQcmdTuples(prepared.result));
			size_t result;
			in >> result;
			return result;
		}

		size_t connection::update_impl(const std::string &stmt) {

			// Prepare statement
			detail::prepared_statement_handle_t prepared = prepare_statement(*_handle, stmt);
			if (!prepared) {

				throw sqlpp::exception("PostgreSQL error: could not store result set");
			}

			// Execute statement
			execute_statement(*_handle, prepared);
			std::istringstream in(PQcmdTuples(prepared.result));
			size_t result;
			in >> result;
			return result;
		}

		size_t connection::remove_impl(const std::string &stmt) {

			// Prepare statement
			detail::prepared_statement_handle_t prepared = prepare_statement(*_handle, stmt);
			if (!prepared) {

				throw sqlpp::exception("PostgreSQL error: could not store result set");
			}

			// Execute statement
			execute_statement(*_handle, prepared);
			std::istringstream in(PQcmdTuples(prepared.result));
			size_t result;
			in >> result;
			return result;
		}

		// prepared execution
		prepared_statement_t connection::prepare_impl(const std::string &stmt) {
			return { std::unique_ptr<detail::prepared_statement_handle_t>(new detail::prepared_statement_handle_t(prepare_statement(*_handle, stmt))) };
		}

		bind_result_t connection::run_prepared_select_impl(prepared_statement_t &prep) {
			execute_statement(*_handle, *prep._handle.get());

			return { prep._handle };
		}

		size_t connection::run_prepared_insert_impl(prepared_statement_t &prep) {
			execute_statement(*_handle, *prep._handle.get());

			std::istringstream in(PQcmdTuples(prep._handle->result));
			size_t result;
			in >> result;
			return result;
		}

		size_t connection::run_prepared_update_impl(prepared_statement_t &prep) {
			execute_statement(*_handle, *prep._handle.get());

			std::istringstream in(PQcmdTuples(prep._handle->result));
			size_t result;
			in >> result;
			return result;
		}

		size_t connection::run_prepared_remove_impl(prepared_statement_t &prep) {
			execute_statement(*_handle, *prep._handle.get());

			std::istringstream in(PQcmdTuples(prep._handle->result));
			size_t result;
			in >> result;
			return result;
		}

		// TODO: Fix escaping.
		std::string connection::escape(const std::string &s) const {
			/*char *escaped = PQescapeLiteral(_handle->postgres, s.c_str(), s.length());
			PQfreemem(escaped);*/
			std::string result(s);
			return std::move(result);
		}

		//! start transaction
		void connection::start_transaction() {
			if (_transaction_active) {
				throw sqlpp::exception("PostgreSQL error: transaction already open");
			}

			auto prepared = prepare_statement(*_handle, "BEGIN");
			execute_statement(*_handle, prepared);
			_transaction_active = true;
		}

		//! commit transaction (or throw transaction if transaction has
		// finished already)
		void connection::commit_transaction() {
			if (!_transaction_active) {
				throw sqlpp::exception("PostgreSQL error: transaction failed or finished.");
			}

			_transaction_active = false;
			auto prepared = prepare_statement(*_handle, "COMMIT");
			execute_statement(*_handle, prepared);
		}

		//! rollback transaction
		void connection::rollback_transaction(bool report) {
			if (!_transaction_active) {
				throw sqlpp::exception("PostgreSQL error: transaction failed or finished.");
			}
			if (report) {
				std::cerr << "PostgreSQL warning: rolling back unfinished transaction" << std::endl;
			}

			_transaction_active = false;
			auto prepared = prepare_statement(*_handle, "ROLLBACK");
			execute_statement(*_handle, prepared);
		}

		//! report rollback failure
		void connection::report_rollback_failure(const std::string &message) noexcept {
			std::cerr << "PostgreSQL error: " << message << std::endl;
		}
	}
}

