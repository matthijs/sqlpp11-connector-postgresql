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

#include <libpq-fe.h>

#include <sqlpp11/postgresql/exception.h>
#include <sqlpp11/postgresql/result.h>

#include <string>


namespace sqlpp
{
namespace postgresql
{

Result::Result() : m_result(nullptr)
{
}

void Result::checkIndex(int record, int field) const noexcept(false)
{
    if (record > records_size() || field > field_count())
    throw std::out_of_range("PostgreSQL error: index out of range");
}

void Result::operator=(PGresult* res)
{
    m_result = res;
    CheckStatus();
}

void Result::CheckStatus() const
{
    const std::string Err = StatusError();
    if (!Err.empty())
    ThrowSQLError(Err, query());
}

[[noreturn]] void Result::ThrowSQLError(const std::string& Err, const std::string& Query) const
{
    // Try to establish more precise error type, and throw corresponding exception
    const char* const code = PQresultErrorField(m_result, PG_DIAG_SQLSTATE);
    if (code)
    switch (code[0])
    {
        case '0':
        switch (code[1])
        {
            case '8':
            throw broken_connection(Err);
            case 'A':
            throw feature_not_supported(Err, Query);
        }
        break;
        case '2':
        switch (code[1])
        {
            case '2':
            throw data_exception(Err, Query);
            case '3':
            if (strcmp(code, "23001") == 0)
                throw restrict_violation(Err, Query);
            if (strcmp(code, "23502") == 0)
                throw not_null_violation(Err, Query);
            if (strcmp(code, "23503") == 0)
                throw foreign_key_violation(Err, Query);
            if (strcmp(code, "23505") == 0)
                throw unique_violation(Err, Query);
            if (strcmp(code, "23514") == 0)
                throw check_violation(Err, Query);
            throw integrity_constraint_violation(Err, Query);
            case '4':
            throw invalid_cursor_state(Err, Query);
            case '6':
            throw invalid_sql_statement_name(Err, Query);
        }
        break;
        case '3':
        switch (code[1])
        {
            case '4':
            throw invalid_cursor_name(Err, Query);
        }
        break;
        case '4':
        switch (code[1])
        {
            case '2':
            if (strcmp(code, "42501") == 0)
                throw insufficient_privilege(Err, Query);
            if (strcmp(code, "42601") == 0)
                throw syntax_error(Err, Query, errorPosition());
            if (strcmp(code, "42703") == 0)
                throw undefined_column(Err, Query);
            if (strcmp(code, "42883") == 0)
                throw undefined_function(Err, Query);
            if (strcmp(code, "42P01") == 0)
                throw undefined_table(Err, Query);
        }
        break;
        case '5':
        switch (code[1])
        {
            case '3':
            if (strcmp(code, "53100") == 0)
                throw disk_full(Err, Query);
            if (strcmp(code, "53200") == 0)
                throw out_of_memory(Err, Query);
            if (strcmp(code, "53300") == 0)
                throw too_many_connections(Err);
            throw insufficient_resources(Err, Query);
        }
        break;

        case 'P':
        if (strcmp(code, "P0001") == 0)
            throw plpgsql_raise(Err, Query);
        if (strcmp(code, "P0002") == 0)
            throw plpgsql_no_data_found(Err, Query);
        if (strcmp(code, "P0003") == 0)
            throw plpgsql_too_many_rows(Err, Query);
        throw plpgsql_error(Err, Query);
    }
    throw sql_error(Err, Query);
}

std::string Result::StatusError() const
{
    if (!m_result)
    throw failure("No result set given");

    std::string Err;

    switch (PQresultStatus(m_result))
    {
    case PGRES_EMPTY_QUERY:  // The string sent to the backend was empty.
    case PGRES_COMMAND_OK:   // Successful completion of a command returning no data
    case PGRES_TUPLES_OK:    // The query successfully executed
        break;

    case PGRES_COPY_OUT:  // Copy Out (from server) data transfer started
    case PGRES_COPY_IN:   // Copy In (to server) data transfer started
        break;

    case PGRES_BAD_RESPONSE:  // The server's response was not understood
    case PGRES_NONFATAL_ERROR:
    case PGRES_FATAL_ERROR:
        Err = PQresultErrorMessage(m_result);
        break;
    case PGRES_COPY_BOTH:
    case PGRES_SINGLE_TUPLE:
        throw sqlpp::exception("pqxx::result: Unrecognized response code " +
                                std::to_string(PQresultStatus(m_result)));
    }
    return Err;
}


int
Result::errorPosition() const noexcept
{
    if (!m_result)
        return -1;

    auto
        p = PQresultErrorField(
                m_result
            ,   PG_DIAG_STATEMENT_POSITION
            );

    if (!p)
        return -1;

    try
    {
        return std::stoi(p);
    }
    catch(...)
    {
        return -1;
    }
}


sqlpp::postgresql::Result::operator bool() const
{
    return m_result != 0;
}

void Result::clear()
{
    if (m_result)
    PQclear(m_result);
    m_result = nullptr;
}


int
Result::affected_rows()
{
    auto
        RowsStr = PQcmdTuples(m_result);

    if (!RowsStr[0])
        return 0;

    try
    {
        return std::stoi(RowsStr);
    }
    catch(...)
    {
        return 0;
    }
}


int Result::records_size() const
{
    return m_result ? PQntuples(m_result) : 0;
}

int Result::field_count() const
{
    return m_result ? PQnfields(m_result) : 0;
}

bool Result::isNull(int record, int field) const
{
    /// check index?
    return PQgetisnull(m_result, record, field);
}

int Result::length(int record, int field) const
{
    /// check index?
    return PQgetlength(m_result, record, field);
}

Result::~Result()
{
    clear();
}

ExecStatusType Result::status()
{
    return PQresultStatus(m_result);
}

}
}
