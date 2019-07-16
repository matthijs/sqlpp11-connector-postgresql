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

#ifndef SQLPP_POSTGRESQL_NO_CONFLICT_H
#define SQLPP_POSTGRESQL_NO_CONFLICT_H

#include <sqlpp11/postgresql/do_nothing.h>
#include <sqlpp11/statement.h>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_on_conflict)

  namespace postgresql
  {
    struct on_conflict_name_t
    {
    };

    struct on_conflict_t : public statement_name_t<on_conflict_name_t, tag::is_on_conflict>
    {
    };

    template <typename Database>
    using blank_on_conflict_t = statement_t<Database, on_conflict_t, no_do_nothing_t>;
    // no_do_update>;

    inline blank_on_conflict_t<void> on_conflict()
    {
      return {};
    }

    struct no_on_conflict_t
    {
      using _traits = make_traits<no_value_t, tag::is_on_conflict>;
      using _nodes = ::sqlpp::detail::type_vector<>;

      using _data_t = no_data_t;

      template <typename Policies>
      struct _impl_t
      {
        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
        _impl_t() = default;
        _impl_t(const _data_t& data) : _data(data)
        {
        }

        _data_t _data;
      };

      template <typename Policies>
      struct _base_t
      {
        using _data_t = no_data_t;

        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
        template <typename... Args>
        _base_t(Args&&... args) : no_on_conflict{std::forward<Args>(args)...}
        {
        }

        _impl_t<Policies> no_on_conflict;
        _impl_t<Policies>& operator()()
        {
          return no_on_conflict;
        }
        const _impl_t<Policies>& operator()() const
        {
          return no_on_conflict;
        }

        template <typename T>
        static auto _get_member(T t) -> decltype(t.no_on_conflict)
        {
          return t.no_on_conflict;
        }

        using _database_t = typename Policies::_database_t;

        template <typename... T>
        static constexpr auto _check_tuple(std::tuple<T...>) -> check_selected_columns_t<T...>
        {
          return {};
        }

        template <typename... T>
        static constexpr auto _check_args(T... args)
            -> decltype(_check_tuple(sqlpp::detail::column_tuple_merge(args...)))
        {
          return {};
        }

        template <typename Check, typename T>
        using _new_statement_t = new_statement_t<Check, Policies, no_on_conflict_t, T>;

        using _consistency_check = consistent_t;

        auto on_conflict() const -> _new_statement_t<consistent_t, do_nothing_t>
        {
        }
      };
    };
  }  // namespace postgresql

  template <typename Context>
  struct serializer_t<Context, postgresql::on_conflict_name_t>
  {
    using _serialize_check = consistent_t;
    using Operand = postgresql::on_conflict_name_t;

    static Context& _(const Operand& o, Context& context)
    {
      context << "ON CONFLICT ";
      return context;
    }
  };
}  // namespace sqlpp

#endif
