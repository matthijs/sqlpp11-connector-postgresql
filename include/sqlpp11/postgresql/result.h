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

#ifndef SQLPP_POSTGRESQL_RESULT_H
#define SQLPP_POSTGRESQL_RESULT_H

#include <libpq-fe.h>
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

      const std::string& query() const
      {
        return m_query;
      }
      std::string& query()
      {
        return m_query;
      }

    private:
      void CheckStatus() const;
      void ThrowSQLError(const std::string& Err, const std::string& Query) const;
      std::string StatusError() const;
      int errorPosition() const throw();

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

#endif
