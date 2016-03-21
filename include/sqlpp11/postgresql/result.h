#pragma once
#include "pgsql/libpq-fe.h"
#include <sstream>
#include <string>
#include <iostream>

#include <boost/lexical_cast.hpp>

namespace sqlpp
{
  namespace postgresql
  {
    class __attribute__((__visibility__("default"))) Result
    {
    public:
      Result();
      ~Result();

      ExecStatusType status();

      void clear();

      size_t affected_rows();
      size_t records_size() const;
      size_t field_count() const;
      size_t length(size_t record, size_t field) const;
      bool isNull(size_t record, size_t field) const;
      void operator=(PGresult* res);
      operator bool() const;

      template <typename T>
      inline T getValue(size_t record, size_t field) const
      {
        static_assert(std::is_arithmetic<T>::value, "Value must be numeric type");
        checkIndexAndThrow(record, field);
        T t(0);
        try
        {
          t = boost::lexical_cast<T>(PQgetvalue(m_result, record, field));
        }
        catch (boost::bad_lexical_cast)
        {
        }
        return t;
      }

      const std::string &query() const { return m_query; }
      std::string &query() { return m_query; }
    private:
      void CheckStatus() const;
      void ThrowSQLError( const std::string &Err, const std::string &Query) const;
      std::string StatusError() const;
      int errorPosition() const throw ();

      void checkIndexAndThrow(size_t record, size_t field) const throw(std::out_of_range);

      PGresult* m_result;
      std::string m_query;
    };

    template <>
    inline const char* Result::getValue<const char*>(size_t record, size_t field) const
    {
      return const_cast<const char*>(PQgetvalue(m_result, record, field));
    }

    template <>
    inline bool Result::getValue<bool>(size_t record, size_t field) const
    {
      checkIndexAndThrow(record, field);
      auto val = PQgetvalue(m_result, record, field);
      if (*val == 't')
        return true;
      else if (*val == 'f')
        return false;
      return const_cast<const char*>(val);
    }
  }
}
