#ifndef SQLPP_POSTGRES_PGEXCEPTION_H
#define SQLPP_POSTGRES_PGEXCEPTION_H

#include <sqlpp11/exception.h>
#include <postgresql/libpq-fe.h>
#include <iostream>
#include <assert.h>

namespace sqlpp {
namespace postgresql {

class pg_error_message{
public:
    pg_error_message( PGresult *res ):
        pg_error_message( PQresultErrorMessage(res) ) {}

    pg_error_message( const char *mes):
        m_message(mes){}

    bool operator == ( const char* e ) const { return m_message == e; }
    bool operator != ( const char* e ) const { return m_message != e; }

    std::string toString() const { return std::string("PostgreSQL ")+ m_message; }

private:
    std::string m_message;
};

class pg_error_class{
public:
    // http://www.postgresql.org/docs/9.4/static/errcodes-appendix.html
    pg_error_class( const char *error_code )
    {
        //only 2 first chars from error code are needed
        m_class_id[0] = error_code[0];
        m_class_id[1] = error_code[1];
    }

    bool isError() const {
        static constexpr auto errors = {
            "03", // SQL Statement Not Yet Complete
            "08", // Connection Exception
            "09", // Triggered Action Exception
            "0A", // Feature Not Supported
            "0B", // Invalid Transaction Initiation
            "0F", // Locator Exception
            "0P", // Invalid Role Specification
            "0Z", // Diagnostics Exception
            "20", // Case Not Found
            "21", // Cardinality Violation
            "22", // Data Exception
            "23", // Integrity Constraint Violation
            "24", // Invalid Cursor State
            "25", // Invalid Transaction State
            "26", // Invalid SQL Statement Name
            "27", // Triggered Data Change Violation
            "28", // Invalid Authorization Specification
            "2B", // Dependent Privilege Descriptors Still Exist
            "2D", // Invalid Transaction Termination
            "2F", // SQL Routine Exception
            "34", // Invalid Cursor Name
            "38", // External Routine Exception
            "39", // External Routine Invocation Exception
            "3B", // Savepoint Exception
            "3D", // Invalid Catalog Name
            "3F", // Invalid Schema Name
            "40", // Transaction Rollback
            "42", // Syntax Error or Access Rule Violation
            "44", // WITH CHECK OPTION Violation
            "53", // Insufficient Resources
            "54", // Program Limit Exceeded
            "55", // Object Not In Prerequisite State
            "57", // Operator Intervention
            "58", // System Error (errors external to PostgreSQL itself)
            "F0", // Configuration File Error
            "HV", // Foreign Data Wrapper Error (SQL/MED)
            "P0", // PL/pgSQL Error
            "XX", // Internal Error
        };

        for(const auto &id: errors){
            if(id == m_class_id)
                return true;
        }
        return false;
    }

    bool isWarning() const {
        static constexpr auto warnings = {
            "01",
            "02"
        };

        for(const auto &id: warnings){
            if(id == m_class_id)
                return true;
        }
        return false;
    }
private:
    char m_class_id[2];
};

class pg_error_code {
public:

    pg_error_code():
        pg_error_code("00000"){}

    pg_error_code(PGresult *res):
        pg_error_code( PQresultErrorField(res, PG_DIAG_SQLSTATE) ){}

    pg_error_code(const char * code):
        m_class(code), m_code(code) {}

    bool operator == ( const char* e ) const { return m_code == e; }
    bool operator != ( const char* e ) const { return m_code != e; }

    std::string toString() const{ return m_code; }

    pg_error_class error_class() const { return m_class; }
private:
    pg_error_class m_class;
    std::string m_code;
};

class pg_exception : public exception
{
public:
    pg_exception( PGresult *res ):
        exception( PQresultErrorMessage(res) ),
        m_message(res),
        m_code(res)
    {}

//    pg_exception( const char *what ):
//        exception( what ){ }

    const char *what() const noexcept override {
        return std::string( exception::what()+ std::string("Error code: ")+ code().toString()+ "\n" ).c_str();
    }

    const pg_error_code &code() const { return m_code; }
    const pg_error_message &message() const { return m_message; }
private:
    pg_error_message m_message;
    pg_error_code m_code;
};

}
}

#endif
