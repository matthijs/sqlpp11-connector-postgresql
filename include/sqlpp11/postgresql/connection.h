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

#ifndef SQLPP_POSTGRESQL_CONNECTION_H
#define SQLPP_POSTGRESQL_CONNECTION_H

#include <sqlpp11/connection.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/postgresql/connection_config.h>
#include <sqlpp11/postgresql/bind_result.h>
#include <sqlpp11/postgresql/prepared_statement.h>

#include <sstream>

struct pg_conn;
typedef struct pg_conn PGconn;

namespace sqlpp {

	namespace postgresql {

		namespace detail {

			// Forward declaration
			struct connection_handle;
		}

		// Forward declaration
		class connection;

		// Context
		struct context_t {
			context_t(const connection &db) : _db(db) {}
			context_t(const connection&&) = delete;

			template<typename T>
				std::ostream &operator<<(T t) {
					return _os << t;
				}

			std::string escape(const std::string &arg);

			std::string str() const {
				return _os.str();
			}

			size_t count() const {
				return _count;
			}

			void pop_count() {
				++_count;
			}

			const connection &_db;
			std::ostringstream _os;
			size_t _count {1};
		};

		// Connection
		class __attribute__((__visibility__("default"))) connection : public sqlpp::connection {
			private:
				std::unique_ptr<detail::connection_handle> _handle;
				bool _transaction_active {false};

				// direct execution
				bind_result_t select_impl(const std::string &stmt);
				size_t insert_impl(const std::string &stmt);
				size_t update_impl(const std::string &stmt);
				size_t remove_impl(const std::string &stmt);

				// prepared execution
				prepared_statement_t prepare_impl(const std::string &stmt, const size_t &paramCount);
				bind_result_t run_prepared_select_impl(prepared_statement_t &prep);
				size_t run_prepared_execute_impl(prepared_statement_t& prep);
				size_t run_prepared_insert_impl(prepared_statement_t &prep);
				size_t run_prepared_update_impl(prepared_statement_t &prep);
				size_t run_prepared_remove_impl(prepared_statement_t &prep);

			public:
				using _prepared_statement_t = prepared_statement_t;
				using _context_t = context_t;
				using _serializer_context_t = _context_t;
				using _interpreter_context_t = _context_t;

				struct _tags {
					using _null_result_is_trivial_value = std::true_type;
				};

				template<typename T>
					static _context_t& _serialize_interpretable(const T& t, _context_t& context) {
					return ::sqlpp::serialize(t, context);
				}

				template<typename T>
					static _context_t& _interpret_interpretable(const T& t, _context_t& context) {
					return ::sqlpp::serialize(t, context);
				}

				// ctor / dtor
				connection(const std::shared_ptr<connection_config> &config);
				~connection();
				connection(const connection &) = delete;
				connection(connection &&) = delete;
				connection &operator=(const connection &) = delete;
				connection &operator=(connection &&) = delete;

				// Select stmt (returns a result)
				template<typename Select>
					bind_result_t select(const Select &s) {
						_context_t ctx (*this);
						serialize(s, ctx);
						return select_impl(ctx.str());
					}

				// Prepared select
				template<typename Select>
					_prepared_statement_t prepare_select(Select &s) {
						_context_t ctx (*this);
						serialize(s, ctx);
						return prepare_impl(ctx.str(), ctx.count() - 1);
					}

				template<typename PreparedSelect>
					bind_result_t run_prepared_select(const PreparedSelect &s) {
						s._bind_params();
						return run_prepared_select_impl(s._prepared_statement);
					}

				// Insert
				template<typename Insert>
					size_t insert(const Insert &i) {
						_context_t ctx (*this);
						serialize(i, ctx);
						return insert_impl(ctx.str());
					}

				template<typename Insert>
					prepared_statement_t prepare_insert(Insert &i) {
						_context_t ctx (*this);
						serialize(i, ctx);
						return prepare_impl(ctx.str(), ctx.count() - 1);
					}

				template<typename PreparedInsert>
					size_t run_prepared_insert(const PreparedInsert &i) {
						i._bind_params();
						return run_prepared_insert_impl(i._prepared_statement);
					}

				// Update
				template<typename Update>
					size_t update(const Update &u) {
						_context_t ctx (*this);
						serialize(u, ctx);
						return update_impl(ctx.str());
					}

				template<typename Update>
					prepared_statement_t prepare_update(Update &u) {
						_context_t ctx (*this);
						serialize(u, ctx);
						return prepare_impl(ctx.str(), ctx.count() - 1);
					}

				template<typename PreparedUpdate>
					size_t run_prepared_update(const PreparedUpdate &u) {
						u._bind_params();
						return run_prepared_update_impl(u._prepared_statement);
					}

				// Remove
				template<typename Remove>
					size_t remove(const Remove &r) {
						_context_t ctx (*this);
						serialize(r, ctx);
						return remove_impl(ctx.str());
					}

				template<typename Remove>
					prepared_statement_t prepare_remove(Remove &r) {
						_context_t ctx (*this);
						serialize(r, ctx);
						return prepare_impl(ctx.str(), ctx.count() - 1);
					}

				template<typename PreparedRemove>
					size_t run_prepared_remove(const PreparedRemove &r) {
						r._bind_params();
						return run_prepared_remove_impl(r._prepared_statement);
					}

				// Execute
				size_t execute(const std::string& command);

				template<typename Execute,
								 typename Enable = typename std::enable_if<not std::is_convertible<Execute, std::string>::value, void>::type>
				size_t execute(const Execute& x)
				{
					_context_t ctx(*this);
					serialize(x, ctx);
					return execute(ctx.str());
				}

				template<typename Execute>
					_prepared_statement_t prepare_execute(Execute& x)
				{
					_context_t ctx(*this);
					serialize(x, ctx);
					return prepare_impl(ctx.str(), ctx.count() - 1);
				}

				template<typename PreparedExecute>
					void run_prepared_execute(const PreparedExecute& x)
				{
					x._prepared_statement._reset();
					x._bind_params();
					return run_prepared_execute_impl(x._prepared_statement);
				}

				// escape argument
				std::string escape(const std::string &s) const;

				//! call run on the argument
				template<typename T>
					auto _run(const T& t, const std::true_type&) -> decltype(t._run(*this)) {
						return t._run(*this);
					}

				template<typename T>
					auto _run(const T& t, const std::false_type&) -> decltype(t._run(*this));

				template<typename T>
					auto operator()(const T& t) -> decltype(t._run(*this))
				{
					sqlpp::run_check_t<T>::_();
					sqlpp::serialize_check_t<_serializer_context_t, T>::_();
					using _ok = sqlpp::logic::all_t<sqlpp::run_check_t<T>::type::value,
						  sqlpp::serialize_check_t<_serializer_context_t, T>::type::value>;
					return _run(t, _ok{});
				}

				//! call prepare on the argument
				template<typename T>
					auto _prepare(const T& t, const std::true_type&) -> decltype(t._prepare(*this))
					{
						return t._prepare(*this);
					}
				template<typename T>
					auto _prepare(const T& t, const std::false_type&) -> decltype(t._prepare(*this));
				template<typename T>
					auto prepare(const T& t) -> decltype(t._prepare(*this))
				{
					sqlpp::prepare_check_t<T>::_();
					sqlpp::serialize_check_t<_serializer_context_t, T>::_();
					using _ok = sqlpp::logic::all_t<sqlpp::prepare_check_t<T>::type::value,
						  sqlpp::serialize_check_t<_serializer_context_t, T>::type::value>;
					return _prepare(t, _ok{});
				}

				//! start transaction
				void start_transaction();

				//! commit transaction (or throw transaction if transaction has
				// finished already)
				void commit_transaction();

				//! rollback transaction
				void rollback_transaction(bool report);

				//! report rollback failure
				void report_rollback_failure(const std::string &message) noexcept;

				//! get the last inserted id for a certain table
				uint64_t last_insert_id(const std::string &table, const std::string &fieldname);

				::PGconn* native_handle();
		};

		inline std::string context_t::escape(const std::string &arg) {
			return _db.escape(arg);
		}
	}
}

#include <sqlpp11/postgresql/serializer.h>

#endif
