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

namespace sqlpp
{
  namespace postgresql
  {
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
        auto txt = std::string(getPqValue(m_result, record, field));
        if(txt != "")

        {
          t = std::stold(txt);
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

      // move PQgetvalue to implementation so we don't depend on the libpq in the
      // public interface
      const char* getPqValue(PGresult* result, int record, int field) const;

      PGresult* m_result;
      std::string m_query;
    };

    template <>
    inline const char* Result::getValue<const char*>(int record, int field) const
    {
      return const_cast<const char*>(getPqValue(m_result, record, field));
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
      auto val = getPqValue(m_result, record, field);
      if (*val == 't')
        return true;
      else if (*val == 'f')
        return false;
      return const_cast<const char*>(val);
    }

    template <>
    inline const uint8_t* Result::getValue<const uint8_t*>(int record, int field) const 
    {
      return reinterpret_cast<const uint8_t*>(getValue<const char*>(record, field));
    }
  }
}

#endif
