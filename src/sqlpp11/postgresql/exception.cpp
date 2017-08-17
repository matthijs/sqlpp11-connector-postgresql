/**
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

#include <sqlpp11/postgresql/exception.h>

using namespace sqlpp::postgresql;

const std::exception& failure::base() const noexcept
{
  return *this;
}

broken_connection::~broken_connection() noexcept = default;
sql_error::~sql_error() noexcept = default;
in_doubt_error::~in_doubt_error() noexcept = default;
feature_not_supported::~feature_not_supported() noexcept = default;
data_exception::~data_exception() noexcept = default;
integrity_constraint_violation::~integrity_constraint_violation() noexcept = default;
not_null_violation::~not_null_violation() noexcept = default;
restrict_violation::~restrict_violation() noexcept = default;
foreign_key_violation::~foreign_key_violation() noexcept = default;
unique_violation::~unique_violation() noexcept = default;
check_violation::~check_violation() noexcept = default;
invalid_cursor_state::~invalid_cursor_state() noexcept = default;
invalid_sql_statement_name::~invalid_sql_statement_name() noexcept = default;
invalid_cursor_name::~invalid_cursor_name() noexcept = default;
syntax_error::~syntax_error() noexcept = default;
undefined_column::~undefined_column() noexcept = default;
undefined_function::~undefined_function() noexcept = default;
undefined_table::~undefined_table() noexcept = default;
insufficient_privilege::~insufficient_privilege() noexcept = default;
insufficient_resources::~insufficient_resources() noexcept = default;
disk_full::~disk_full() noexcept = default;
out_of_memory::~out_of_memory() noexcept = default;
too_many_connections::~too_many_connections() noexcept = default;
plpgsql_error::~plpgsql_error() noexcept = default;
plpgsql_raise::~plpgsql_raise() noexcept = default;
plpgsql_no_data_found::~plpgsql_no_data_found() noexcept = default;
plpgsql_too_many_rows::~plpgsql_too_many_rows() noexcept = default;
