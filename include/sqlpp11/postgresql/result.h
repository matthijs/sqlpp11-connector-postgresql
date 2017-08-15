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

#include <iostream>
#include <sstream>
#include <string>

#include <libpq-fe.h>

#include <sqlpp11/postgresql/visibility.h>

#include <boost/lexical_cast.hpp>

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

    class DLL_PUBLIC Result
    {
    public:
      Result();
      ~Result();

      ExecStatusType status();

      void clear();

      int affected_rows();
      int records_size() const;
      int field_count() const;
      int length(int record, int field) const;
      bool isNull(int record, int field) const;
      void operator=(PGresult* res);
      operator bool() const;

      template <typename T = const char*>
      inline T getValue(int record, int field) const
      {
        static_assert(std::is_arithmetic<T>::value, "Value must be numeric type");
        checkIndex(record, field);
        T t(0);
        try
        {
            auto val = PQgetvalue(m_result, record, field);

          t = boost::lexical_cast<T>(val);
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
      [[noreturn]] void ThrowSQLError(const std::string& Err, const std::string& Query) const;
      std::string StatusError() const;
      int errorPosition() const noexcept;
      bool hasError();
      void checkIndex(int record, int field) const noexcept(false);

      PGresult* m_result;
      std::string m_query;
    };

    template <>
    inline const char* Result::getValue<const char*>(int record, int field) const
    {
        auto v = PQgetvalue(m_result, record, field);
      return const_cast<const char*>(v);
    }

    template <>
    inline std::string Result::getValue<std::string>(int record, int field) const
    {
      return {getValue<const char*>(record, field)};
    }

    template <>
    inline bool Result::getValue<bool>(int record, int field) const
    {
      checkIndex(record, field);
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
