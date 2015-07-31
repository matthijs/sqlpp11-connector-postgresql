#ifndef SQLPP_POSTGRES_PGEXCEPTION_H
#define SQLPP_POSTGRES_PGEXCEPTION_H

#include <sqlpp11/exception.h>
#include <postgresql/libpq-fe.h>
#include <iostream>

namespace sqlpp {
namespace postgresql {

class pg_error_message{
public:
    pg_error_message( PGresult *res ):
        pg_error_message( PQresultErrorMessage(res) )
    {
    }

    pg_error_message( const char *mes):
        m_message(mes)
    {
    }

    std::string toString() const {
        return std::string("PostgreSQL ")+ m_message;
    }

private:
    std::string m_message;
};

class pg_error_code {
public:
    pg_error_code():
        pg_error_code("00000")
    {
    }

    pg_error_code(const char * code):
        m_code(code)
    {
    }

    std::string toString() const{
        return m_code;
    }

private:
    std::string m_code;
};

class pg_exception : public exception
{
public:
    pg_exception( PGresult *res ):
        pg_exception( PQresultErrorMessage(res), PQresultErrorField(res, PG_DIAG_SQLSTATE) )
    {
    }

    pg_exception( const char *what ):
        exception( what ){

    }

    const char *what() const noexcept override {
        return std::string( exception::what()+ std::string("Error code: ")+ code().toString()+ "\n" ).c_str();
    }

    const pg_error_code &code() const{
        return m_code;
    }

private:
    pg_exception(const pg_error_message &message, const pg_error_code &code):
        exception( message.toString() ), m_code(code)
    {
    }

    pg_error_code m_code;
};

}
}

#endif
