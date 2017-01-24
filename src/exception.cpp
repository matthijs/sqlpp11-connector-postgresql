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

failure::failure(const std::string& whatarg) : sqlpp::exception(whatarg)
{
}

broken_connection::broken_connection() : failure("Connection to database failed")
{
}

broken_connection::broken_connection(const std::string& whatarg) : failure(whatarg)
{
}

sql_error::sql_error() : failure("Failed query"), m_Q()
{
}

sql_error::sql_error(const std::string& whatarg) : failure(whatarg), m_Q()
{
}

sql_error::sql_error(const std::string& whatarg, const std::string& Q) : failure(whatarg), m_Q(Q)
{
}

sql_error::~sql_error() throw()
{
}

const std::string& sql_error::query() const throw()
{
  return m_Q;
}

in_doubt_error::in_doubt_error(const std::string& whatarg) : failure(whatarg)
{
}
