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
	  struct on_conflict_do_nothing_t : public statement_name_t<on_conflict_do_nothing_t, tag::is_on_conflict_do_nothing>
	  {
	  };
  }

  template <typename Context>
  struct serializer_t<Context, postgresql::on_conflict_do_nothing_t>
  {
    using _serialize_check = consistent_t;
    using Operand = postgresql::on_conflict_do_nothing_t;

    static Context& _(const Operand& o, Context& context)
    {
      context << " ON CONFLICT DO NOTHING";
      return context;
    }
  };
}  // namespace sqlpp

#endif
