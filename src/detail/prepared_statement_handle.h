/**
 * Copyright © 2014-2015, Matthijs Möhlmann
 * Copyright © 2015-2016, Bartosz Wieczorek
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

#ifndef SQLPP_POSTGRESQL_PREPARED_STATEMENT_HANDLE_H
#define SQLPP_POSTGRESQL_PREPARED_STATEMENT_HANDLE_H

#include <iostream>
#include <string>
#include <vector>

#include <libpq-fe.h>
#include <sqlpp11/postgresql/result.h>
#include <sqlpp11/postgresql/visibility.h>

#include "connection_handle.h"

#ifdef SQLPP_DYNAMIC_LOADING
#include <sqlpp11/postgresql/dynamic_libpq.h>
#endif

namespace sqlpp
{
  namespace postgresql
  {
#ifdef SQLPP_DYNAMIC_LOADING
    using namespace dynamic;
#endif
    namespace detail
    {
      struct DLL_PUBLIC statement_handle_t
      {
        detail::connection_handle& connection;
        Result result;
        bool valid{false};
        uint32_t count{0};
        uint32_t totalCount = {0};
        uint32_t fields = {0};

        // ctor
        statement_handle_t(detail::connection_handle& _connection);
        statement_handle_t(const statement_handle_t&) = delete;
        statement_handle_t(statement_handle_t&&) = default;
        statement_handle_t& operator=(const statement_handle_t&) = delete;
        statement_handle_t& operator=(statement_handle_t&&) = default;

        virtual ~statement_handle_t();
        bool operator!() const;
        void clearResult();

        bool debug() const;
      };

      struct DLL_PUBLIC prepared_statement_handle_t : public statement_handle_t
      {
      private:
        std::string _name{"xxxxxx"};

      public:
        // Store prepared statement arguments
        std::vector<bool> nullValues;
        std::vector<std::string> paramValues;

        // ctor
        prepared_statement_handle_t(detail::connection_handle& _connection, std::string stmt, const size_t& paramCount);
        prepared_statement_handle_t(const prepared_statement_handle_t&) = delete;
        prepared_statement_handle_t(prepared_statement_handle_t&&) = default;
        prepared_statement_handle_t& operator=(const prepared_statement_handle_t&) = delete;
        prepared_statement_handle_t& operator=(prepared_statement_handle_t&&) = default;

        virtual ~prepared_statement_handle_t();

        void execute();

        std::string name() const
        {
          return _name;
        }

      private:
        void generate_name();
        void prepare(std::string stmt);
      };
    }
  }
}

#endif
