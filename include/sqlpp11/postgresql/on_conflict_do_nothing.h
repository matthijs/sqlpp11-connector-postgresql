/**
 * Copyright © 2014-2019, Matthijs Möhlmann
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

#ifndef SQLPP_POSTGRESQL_ON_CONFLICT_DO_NOTHING_H
#define SQLPP_POSTGRESQL_ON_CONFLICT_DO_NOTHING_H

#include <sqlpp11/detail/type_vector.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_on_conflict_do_nothing)

  namespace postgresql
  {
    struct on_conflict_do_nothing_name_t
    {
    };
    struct on_conflict_do_nothing_t
        : public statement_name_t<on_conflict_do_nothing_name_t, tag::is_on_conflict_do_nothing>
    {
    };
  }  // namespace postgresql

  template <typename Context>
  struct serializer_t<Context, postgresql::on_conflict_do_nothing_name_t>
  {
    using _serialize_check = consistent_t;
    using Operand = postgresql::on_conflict_do_nothing_name_t;

    static Context& _(const Operand& o, Context& context)
    {
      context << " ON CONFLICT DO NOTHING";
      return context;
    }
  };
}  // namespace sqlpp

#endif
