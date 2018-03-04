/**
 * Copyright © 2014-2015, Matthijs Möhlmann
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

#include <sqlpp11/exception.h>
#include <sqlpp11/postgresql/bind_result.h>

#include <date/date.h>
#include <iostream>
#include <sstream>

#include "detail/prepared_statement_handle.h"

#if defined(_WIN32) || defined(_WIN64)
#pragma warning (disable:4800)  // int to bool
#endif

namespace sqlpp
{
  namespace postgresql
  {
    bind_result_t::bind_result_t(const std::shared_ptr<detail::statement_handle_t>& handle) : _handle(handle)
    {
      if (this->_handle && this->_handle->debug())
      {
        // cerr
        std::cerr << "PostgreSQL debug: constructing bind result, using handle at: " << this->_handle.get()
                  << std::endl;
      }
    }

    bool bind_result_t::next_impl()
    {
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: accessing next row of handle at " << _handle.get() << std::endl;
      }

      // Fetch total amount
      if (_handle->totalCount == 0U)
      {
        _handle->totalCount = _handle->result.records_size();
        if (_handle->totalCount == 0U)
          return false;
      }
      else
      {
        // Next row
        if (_handle->count < (_handle->totalCount - 1))
        {
          _handle->count++;
        }
        else
        {
          return false;
        }
      }

      // Really needed?
      if (_handle->fields == 0U)
      {
        _handle->fields = _handle->result.field_count();
      }

      return true;
    }

    void bind_result_t::_bind_boolean_result(size_t _index, signed char* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding boolean result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);
      *value = _handle->result.getValue<bool>(_handle->count, index);
    }

    void bind_result_t::_bind_floating_point_result(size_t _index, double* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding floating_point result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);
      *value = _handle->result.getValue<double>(_handle->count, index);
    }

    void bind_result_t::_bind_integral_result(size_t _index, int64_t* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding integral result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);
      *value = _handle->result.getValue<unsigned long long>(_handle->count, index);
    }

    void bind_result_t::_bind_text_result(size_t _index, const char** value, size_t* len)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding text result at index: " << index << std::endl;
      }

      if (_handle->result.isNull(_handle->count, index))
      {
        *value = nullptr;
        *len = 0;
      }
      else
      {
        *value = _handle->result.getValue<const char*>(_handle->count, index);
        *len = _handle->result.length(_handle->count, index);
      }
    }

    // same parsing logic as SQLite connector
    // PostgreSQL will return one of those (using the default ISO client):
    //
    // 2010-10-11 01:02:03 - ISO timestamp without timezone
    // 2011-11-12 01:02:03.123456 - ISO timesapt with sub-second (microsecond) precision
    // 1997-12-17 07:37:16-08 - ISO timestamp with timezone
    // 1992-10-10 01:02:03-06:30 - for some timezones with non-hour offset
    // 1900-01-01 - date only
    // we do not support time-only values !
    namespace
    {
      const auto date_digits = std::vector<char>{1, 1, 1, 1, 0, 1, 1, 0, 1, 1};  // 2016-11-10
      const auto time_digits = std::vector<char>{0, 1, 1, 0, 1, 1, 0, 1, 1};     // ' 13:12:11'
      const auto tz_digits = std::vector<char>{0, 1, 1};                         // -05
      const auto tz_min_digits = std::vector<char>{0, 1, 1};                     // :30

      auto check_digits(const char* text, const std::vector<char>& digitFlags) -> bool
      {
        for (const auto digitFlag : digitFlags)
        {
          if (digitFlag)
          {
            if (not std::isdigit(*text))
            {
              return false;
            }
          }
          else
          {
            if (std::isdigit(*text) or *text == '\0')
            {
              return false;
            }
          }
          ++text;
        }
        return true;
      }
    }

    void bind_result_t::_bind_date_result(size_t _index, ::sqlpp::chrono::day_point* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);

      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding date result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);

      if (!(*is_null))
      {
        const auto date_string = _handle->result.getValue<const char*>(_handle->count, index);

        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: date string: " << date_string << std::endl;
        }
        auto len = static_cast<size_t>(_handle->result.length(_handle->count, index));

        if (len >= date_digits.size() && check_digits(date_string, date_digits))
        {
          const auto ymd =
              ::date::year(std::atoi(date_string)) / std::atoi(date_string + 5) / std::atoi(date_string + 8);
          *value = ::sqlpp::chrono::day_point(ymd);
        }
        else
        {
          if (_handle->debug())
            std::cerr << "PostgreSQL debug: got invalid date '" << date_string << "'" << std::endl;
          *value = {};
        }
      }
      else
      {
        *value = {};
      }
    }

    // always returns local time for timestamp with time zone
    void bind_result_t::_bind_date_time_result(size_t _index, ::sqlpp::chrono::microsecond_point* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding date_time result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);

      if (!(*is_null))
      {
        const auto date_string = _handle->result.getValue(_handle->count, index);

        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: got date_time string: " << date_string << std::endl;
        }
        auto len = static_cast<size_t>(_handle->result.length(_handle->count, index));
        if (len >= date_digits.size() && check_digits(date_string, date_digits))
        {
          const auto ymd =
              ::date::year(std::atoi(date_string)) / std::atoi(date_string + 5) / std::atoi(date_string + 8);
          *value = ::sqlpp::chrono::day_point(ymd);
        }
        else
        {
          if (_handle->debug())
            std::cerr << "PostgreSQL debug: got invalid date_time" << std::endl;
          *value = {};
          return;
        }

        auto date_time_size = date_digits.size() + time_digits.size();
        const auto time_string = date_string + date_digits.size();
        if ((len >= date_time_size) && check_digits(date_string, date_digits))
        {
          // not the ' ' (or standard: 'T') prefix for times
          *value += std::chrono::hours(std::atoi(time_string + 1)) + std::chrono::minutes(std::atoi(time_string + 4)) +
                    std::chrono::seconds(std::atoi(time_string + 7));
        }
        else
        {
          return;
        }

        bool has_ms = false;
        if ((len > date_time_size) && (time_string[time_digits.size()] == '.'))
        {
          has_ms = true;
          const auto ms_string = time_string + time_digits.size() + 1;

          int digits_count = 0;
          while (ms_string[digits_count] != '\0' && ms_string[digits_count] != '-' && ms_string[digits_count] != '+')
          {
            if (!std::isdigit(ms_string[digits_count]))
            {
              if (_handle->debug())
                std::cerr << "PostgreSQL debug: got invalid date_time" << std::endl;
              *value = {};
              return;
            }

            ++digits_count;
          }

          if (digits_count == 0)
          {
            if (_handle->debug())
              std::cerr << "PostgreSQL debug: got invalid date_time" << std::endl;
            *value = {};
            return;
          }

          date_time_size += digits_count;

          int pg_ms_num = std::atoi(ms_string);

          while (digits_count++ < 6)
            pg_ms_num *= 10;

          *value += std::chrono::microseconds(pg_ms_num);
        }
        if (len >= (date_time_size + tz_digits.size()))
        {
          const auto tz_string = date_string + date_time_size;
          const auto zone_hour = std::atoi(tz_string);
          auto zone_min = 0;

          if ((len >= date_time_size + tz_digits.size() + tz_min_digits.size()) &&
              check_digits(tz_string + tz_digits.size(), tz_min_digits))
          {
            zone_min = std::atoi(tz_string + tz_digits.size() + 1);
          }
          // ignore -00:xx, as there currently is no timezone using it, and hopefully never will be
          if (zone_hour >= 0)
          {
            //*value += std::chrono::hours(zone_hour) + std::chrono::minutes(zone_min);
          }
          else
          {
            //*value += std::chrono::hours(zone_hour) - std::chrono::minutes(zone_min);
          }
        }
        if (_handle->debug())
        {
          auto ts = std::chrono::system_clock::to_time_t(*value);
          std::tm* tm = std::localtime(&ts);
          std::string time_str{"1900-01-01 00:00:00 CEST"};
          strftime(const_cast<char*>(time_str.data()), time_str.size(), "%F %T %Z", tm);
          std::cerr << "PostgreSQL debug: calculated timestamp " << time_str << std::endl;
        }
      }
      else
      {
        *value = {};
      }
    }
  }
}
