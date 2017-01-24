/**
 * Copyright © 2014-2015, Matthijs Möhlmann
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

#include <iostream>
#include <sstream>

#include "detail/prepared_statement_handle.h"

namespace sqlpp
{
  namespace postgresql
  {
    bind_result_t::bind_result_t(const std::shared_ptr<detail::statement_handle_t>& handle) : _handle(handle)
    {
      if (this->_handle && this->_handle->debug)
      {
        std::cerr << "PostgreSQL debug: constructing bind result, using handle at: " << this->_handle.get()
                  << std::endl;
      }
    }

    bool bind_result_t::next_impl()
    {
      if (_handle->debug)
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

    void bind_result_t::_bind_boolean_result(size_t index, signed char* value, bool* is_null)
    {
      if (_handle->debug)
      {
        std::cerr << "PostgreSQL debug: binding boolean result at index: " << index << std::endl;
      }

      // Assign value
      const auto& res = _handle->result;
      *value = res.getValue<bool>(_handle->count, index);
      *is_null = res.isNull(_handle->count, index);
    }

    void bind_result_t::_bind_floating_point_result(size_t index, double* value, bool* is_null)
    {
      if (_handle->debug)
      {
        std::cerr << "PostgreSQL debug: binding floating_point result at index: " << index << std::endl;
      }

      const auto& res = _handle->result;
      *value = res.getValue<double>(_handle->count, index);
      *is_null = res.isNull(_handle->count, index);
    }

    void bind_result_t::_bind_integral_result(size_t index, int64_t* value, bool* is_null)
    {
      if (_handle->debug)
      {
        std::cerr << "PostgreSQL debug: binding integral result at index: " << index << std::endl;
      }

      const auto& res = _handle->result;
      *value = res.getValue<int64_t>(_handle->count, index);
      *is_null = res.isNull(_handle->count, index);
    }

    void bind_result_t::_bind_text_result(size_t index, const char** value, size_t* len)
    {
      if (_handle->debug)
      {
        std::cerr << "PostgreSQL debug: binding text result at index: " << index << std::endl;
      }

      const auto& res = _handle->result;
      *value = res.getValue<const char*>(_handle->count, index);
      *len = res.length(_handle->count, index);
    }
  }
}
