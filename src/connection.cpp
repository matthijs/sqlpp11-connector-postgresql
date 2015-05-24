/**
 * Copyright © 2014-2015, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/exception.h>

#include <algorithm>
#include <iostream>

#include "detail/prepared_statement_handle.h"
#include "detail/connection_handle.h"

namespace sqlpp {

	namespace postgresql {

		namespace {
			detail::prepared_statement_handle_t prepare_statement(detail::connection_handle &handle, const std::string &stmt, const size_t &paramCount) {
				if (handle.config->debug) {
					std::cerr << "PostgreSQL debug: preparing: " << stmt << std::endl;
				}

				detail::prepared_statement_handle_t result(handle.postgres, paramCount, handle.config->debug);

				// Generate a random name for the prepared statement
				while(std::find(handle.prepared_statement_names.begin(), handle.prepared_statement_names.end(), result.name) != handle.prepared_statement_names.end()) {
					std::generate_n(result.name.begin(), 6, []() {
							const char charset[] =
								"0123456789"
								"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								"abcdefghijklmnopqrstuvwxyz";
							constexpr size_t max = (sizeof(charset) - 1);
							std::random_device rd;
							return charset[rd() % max];
						});
				}
				handle.prepared_statement_names.push_back(result.name);

				// Create the prepared statement
				PGresult *res = PQprepare(handle.postgres,
						result.name.c_str(),
						stmt.c_str(),
						0,
						nullptr);
				std::string errmsg = "PostgreSQL error: ";
				ExecStatusType ret = PQresultStatus(res);
				std::string rerrmsg(PQresultErrorMessage(res));
				PQclear(res);
				switch(ret) {
					case PGRES_EMPTY_QUERY:
					case PGRES_COPY_OUT:
					case PGRES_COPY_IN:
					case PGRES_BAD_RESPONSE:
					case PGRES_NONFATAL_ERROR:
					case PGRES_FATAL_ERROR:
					case PGRES_COPY_BOTH:
						errmsg.append(std::string(PQresStatus(ret)) + std::string(": ") + rerrmsg);
						throw sqlpp::exception(errmsg);
					case PGRES_COMMAND_OK:
					case PGRES_TUPLES_OK:
					case PGRES_SINGLE_TUPLE:
					default:
						result.valid = true;
						break;
				}

				return result;
			}

			void execute_statement(detail::connection_handle &handle, detail::prepared_statement_handle_t &prepared) {

				// Execute a prepared statement
				char *paramValues[prepared.paramValues.size()];
				//int paramLengths[prepared.paramValues.size()];
				for (uint32_t i = 0; i < prepared.paramValues.size(); i++) {
					if (!prepared.nullValues[i]) {
						paramValues[i] = const_cast<char *>(prepared.paramValues[i].c_str());
						//paramLengths[i] = prepared.paramValues[i].size();
					} else {
						paramValues[i] = nullptr;
						//paramLengths[i] = 0;
					}
				}

				// Execute prepared statement with the parameters.
				if (prepared.result) {
					PQclear(prepared.result);
					prepared.result = nullptr;
				}
				prepared.count = 0;
				prepared.totalCount = 0;
				prepared.result = PQexecPrepared(handle.postgres,
						prepared.name.c_str(),
						prepared.paramValues.size(),
						paramValues,
						nullptr,
						nullptr,
						0);

				// check statement
				std::string errmsg = "PostgreSQL error: ";
				ExecStatusType ret = PQresultStatus(prepared.result);
				switch(ret) {
					case PGRES_EMPTY_QUERY:
					case PGRES_COPY_OUT:
					case PGRES_COPY_IN:
					case PGRES_BAD_RESPONSE:
					case PGRES_NONFATAL_ERROR:
					case PGRES_FATAL_ERROR:
					case PGRES_COPY_BOTH:
						prepared.valid = false;
						errmsg.append(std::string(PQresStatus(ret)) + std::string(": ") + std::string(PQresultErrorMessage(prepared.result)));
						throw sqlpp::exception(errmsg);
					case PGRES_COMMAND_OK:
					case PGRES_TUPLES_OK:
					case PGRES_SINGLE_TUPLE:
					default:
						prepared.valid = true;
						break;
				}
			}
		}

		connection::connection(const std::shared_ptr<connection_config> &config) : _handle(new detail::connection_handle(config)) {
		}

		connection::~connection() {
		}

		// direct execution
		bind_result_t connection::select_impl(const std::string &stmt) {

			// Prepare statement
			std::unique_ptr<detail::prepared_statement_handle_t> prepared(new detail::prepared_statement_handle_t(prepare_statement(*_handle, stmt, 0)));
			if (!prepared) {

				throw sqlpp::exception("PostgreSQL error: could not store result set");
			}

			// Execute statement
			execute_statement(*_handle, *prepared);
			return {std::move(prepared)};
		}

		size_t connection::insert_impl(const std::string &stmt) {

			// Prepare statement
			detail::prepared_statement_handle_t prepared = prepare_statement(*_handle, stmt, 0);
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
			detail::prepared_statement_handle_t prepared = prepare_statement(*_handle, stmt, 0);
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
			detail::prepared_statement_handle_t prepared = prepare_statement(*_handle, stmt, 0);
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
		prepared_statement_t connection::prepare_impl(const std::string &stmt, const size_t &paramCount) {
			return { std::unique_ptr<detail::prepared_statement_handle_t>(new detail::prepared_statement_handle_t(prepare_statement(*_handle, stmt, paramCount))) };
		}

		bind_result_t connection::run_prepared_select_impl(prepared_statement_t &prep) {
			execute_statement(*_handle, *prep._handle.get());

			return { prep._handle };
		}

		size_t connection::run_prepared_execute_impl(prepared_statement_t &prep) {
			execute_statement(*_handle, *prep._handle.get());

			std::istringstream in(PQcmdTuples(prep._handle->result));
			size_t result;
			in >> result;
			return result;
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

			// Escape strings
			char to[(s.size() * 2) + 1];
			int err;
			size_t length = PQescapeStringConn(_handle->postgres, to, s.c_str(), s.size(), &err);
			std::string result(to, length);
			return std::move(result);
		}

		//! start transaction
		void connection::start_transaction() {
			if (_transaction_active) {
				throw sqlpp::exception("PostgreSQL error: transaction already open");
			}

			auto prepared = prepare_statement(*_handle, "BEGIN", 0);
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
			auto prepared = prepare_statement(*_handle, "COMMIT", 0);
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
			auto prepared = prepare_statement(*_handle, "ROLLBACK", 0);
			execute_statement(*_handle, prepared);
		}

		//! report rollback failure
		void connection::report_rollback_failure(const std::string &message) noexcept {
			std::cerr << "PostgreSQL error: " << message << std::endl;
		}

		uint64_t connection::last_insert_id(const std::string &table, const std::string &fieldname) {
			std::string sql = "SELECT currval('" + table + "_" + fieldname + "_seq')";
			PGresult *res = PQexec(_handle->postgres, sql.c_str());

			// Parse the number and return.
			// TODO: A copy is happening here, how to prevent that?
			std::string in {PQgetvalue(res, 0, 0)};
			PQclear(res);
			return std::stoi(in);
		}

		::PGconn* connection::native_handle() {
			return _handle->postgres;
		}
	}
}

