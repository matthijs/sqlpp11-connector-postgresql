#include <sqlpp11/exception.h>
#include <postgresql/libpq-fe.h>

#ifndef SQLPP_POSTGRES_PGEXCEPTION_H
#define SQLPP_POSTGRES_PGEXCEPTION_H


namespace sqlpp {
namespace postgresql {

class PgException : public exception
{
public:
    PgException(const char* field, const char *what_arg):
        exception(what_arg), m_field(field)
    {
    }

    PgException(const std::string& what_arg):
        exception(what_arg) {}
    PgException(const char* what_arg):
        exception(what_arg) {}
    const char *what() const noexcept override {
        return std::string(m_field + exception::what() ).c_str();
    }

private:
    std::string m_field;
};

}
}

#endif
