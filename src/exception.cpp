#include <sqlpp11/postgresql/exception.h>

using namespace sqlpp::postgresql;

failure::failure(const std::string &whatarg) :
  sqlpp::exception(whatarg)
{
}

broken_connection::broken_connection() :
  failure("Connection to database failed")
{
}

broken_connection::broken_connection(const std::string &whatarg) :
  failure(whatarg)
{
}

sql_error::sql_error() :
  failure("Failed query"),
  m_Q()
{
}

sql_error::sql_error(const std::string &whatarg) :
  failure(whatarg),
  m_Q()
{
}

sql_error::sql_error(const std::string &whatarg,
    const std::string &Q) :
  failure(whatarg),
  m_Q(Q)
{
}

sql_error::~sql_error() throw ()
{
}


const std::string & sql_error::query() const throw ()
{
  return m_Q;
}


in_doubt_error::in_doubt_error(const std::string &whatarg) :
  failure(whatarg)
{
}
