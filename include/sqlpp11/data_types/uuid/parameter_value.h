/**
 * Copyright (c) 2020, Matthijs Möhlmann <matthijs@cacholong.nl>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
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

#ifndef SQLPP_UUID_PARAMETER_VALUE_H
#define SQLPP_UUID_PARAMETER_VALUE_H

#include <sqlpp11/data_types/parameter_value.h>
#include <sqlpp11/data_types/parameter_value_base.h>
#include <sqlpp11/data_types/uuid/data_type.h>
#include <sqlpp11/tvin.h>

#include <boost/uuid/uuid_io.hpp>

namespace sqlpp
{
  template <>
  struct parameter_value_t<uuid> : public parameter_value_base<uuid>
  {
    using base = parameter_value_base<uuid>;
    using base::base;
    using base::operator=;

    template <typename Target>
    void _bind(Target& target, size_t index) const
    {
      std::string val = boost::uuids::to_string(this->_value);
      target._bind_text_parameter(index, &val, _is_null);
    }
  };
}  // namespace sqlpp

#endif