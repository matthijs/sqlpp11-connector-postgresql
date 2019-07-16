#ifndef SQLPP_POSTGRESQL_DO_NOTHING_H
#define SQLPP_POSTGRESQL_DO_NOTHING_H

#include <sqlpp11/detail/type_vector.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_do_nothing)

  namespace postgresql
  {
    struct no_do_nothing_t
    {
      using _traits = make_traits<no_value_t, tag::is_noop>;
      using _nodes = sqlpp::detail::type_vector<>;

      // Data
      using _data_t = no_data_t;

      // Member implementation with data and methods
      template <typename Policies>
      struct _impl_t
      {
        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
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

        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
        template <typename... Args>
        _base_t(Args&&... args) : no_do_nothing{std::forward<Args>(args)...}
        {
        }

        _impl_t<Policies> no_do_nothing;
        _impl_t<Policies>& operator()()
        {
          return no_do_nothing;
        }
        const _impl_t<Policies>& operator()() const
        {
          return no_do_nothing;
        }

        using _consistency_check = consistent_t;
      };
    };

    struct do_nothing_t
    {
      using _traits = make_traits<no_value_t, tag::is_do_nothing>;
      using _nodes = sqlpp::detail::type_vector<>;

      // Data
      using _data_t = no_data_t;

      // Member implementation with data and methods
      template <typename Policies>
      struct _impl_t
      {
        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
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

        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
        template <typename... Args>
        _base_t(Args&&... args) : do_nothing{std::forward<Args>(args)...}
        {
        }
        _impl_t<Policies> do_nothing;
        _impl_t<Policies>& operator()()
        {
          return do_nothing;
        }
        const _impl_t<Policies>& operator()() const
        {
          return do_nothing;
        }

        using _consistency_check = consistent_t;
      };
    };
  }  // namespace postgresql

  template <typename Context>
  struct serializer_t<Context, postgresql::do_nothing_t>
  {
    // using _serialize_check = ;
    using Operand = postgresql::do_nothing_t;

    static Context& _(const Operand& o, Context& context)
    {
      context << "DO NOTHING";
      return context;
    }
  };
}  // namespace sqlpp

#endif
