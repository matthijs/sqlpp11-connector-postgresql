#pragma once

#include <sqlpp11/statement.h>
#include <sqlpp11/connection.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_insert.h>
#include <sqlpp11/default_value.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/into.h>
#include <sqlpp11/insert_value_list.h>

#include <sqlpp11/postgresql/returning_column_list.h>

namespace sqlpp{
namespace postgresql{

struct insert_name_t {};
struct insert_t: public statement_name_t<insert_name_t>
{
    using _traits = make_traits<no_value_t, tag::is_return_value>;
    struct _alias_t {};

    template<typename Statement>
        struct _result_methods_t
        {
            using _statement_t = Statement;

            const _statement_t& _get_statement() const
            {
                return static_cast<const _statement_t&>(*this);
            }

            // Execute
            template<typename Db, typename Composite>
                auto _run(Db& db, const Composite& composite) const
                -> decltype(db.insert(composite))
                {
                    return db.insert(composite);
                }

            template<typename Db>
                auto _run(Db& db) const -> decltype(db.insert(this->_get_statement()))
                {
                    return db.insert(_get_statement());
                }

            // Prepare
            template<typename Db, typename Composite>
                auto _prepare(Db& db, const Composite& composite) const
                -> prepared_insert_t<Db, Composite>
                {
                    return {{}, db.prepare_insert(composite)};
                }

            template<typename Db>
                auto _prepare(Db& db) const
                -> prepared_insert_t<Db, _statement_t>
                {
                    return {{}, db.prepare_insert(_get_statement())};
                }
        };
};

//struct no_returning_t
//{
//    using _traits = make_traits<no_value_t, tag::is_noop>;
//    using _nodes = sqlpp::detail::type_vector<>;

//    // Data
//    using _data_t = no_data_t;

//    // Member implementation with data and methods
//    template<typename Policies>
//        struct _impl_t
//        {
//            _data_t _data;
//        };

//        struct assert_returning_t
//        {
//            using type = std::false_type;

//            template<typename T = void>
//                static void _()
//                {
//                    static_assert(wrong_t<T>::value, "into() required");
//                }
//        };

//    // Base template to be inherited by the statement
//    template<typename Policies>
//        struct _base_t
//        {
//            using _data_t = no_data_t;

//            _impl_t<Policies> no_returning;
//            _impl_t<Policies>& operator()() { return no_returning; }
//            const _impl_t<Policies>& operator()() const { return no_returning; }

//            template<typename T>
//                static auto _get_member(T t) -> decltype(t.no_returning)
//                {
//                    return t.no_returning;
//                }

//            using _database_t = typename Policies::_database_t;

//            template<typename T>
//                using _check = logic::all_t<is_raw_table_t<T>::value>;

//            template<typename Check, typename T>
//                using _new_statement_t = new_statement_t<Check::value, Policies, no_into_t, T>;

//            using _consistency_check = assert_returning_t;

//            template<typename ... Columns>
//                auto returning(Columns ... columns) const
//                -> _new_statement_t<_check<columns>, returning_t<void, columns>>
//                {
//                    static_assert(_check<columns>::value, "argument is not a column in returning()");
//                    return _into_impl<void>(_check<Table>{}, table);
//                }

//        private:
//            template<typename Database, typename Table>
//                auto _into_impl(const std::false_type&, Table table) const
//                -> bad_statement;

//            template<typename Database, typename Table>
//                auto _into_impl(const std::true_type&, Table table) const
//                -> _new_statement_t<std::true_type, into_t<Database, Table>>
//                {
//                    static_assert(required_tables_of<into_t<Database, Table>>::size::value == 0, "argument depends on another table in into()");

//                    return { static_cast<const derived_statement_t<Policies>&>(*this), into_data_t<Database, Table>{table} };
//                }
//        };
//};

template<typename Database>
    using blank_insert_t = statement_t<Database,
                insert_t,
                no_into_t,
                no_insert_value_list_t,
//                no_returning_t,
                no_returning_column_list_t>;

inline auto insert()
    -> blank_insert_t<void>
    {
        return { blank_insert_t<void>() };
    }

template<typename Table>
    constexpr auto insert_into(Table table)
    -> decltype(blank_insert_t<void>().into(table))
    {
        return { blank_insert_t<void>().into(table) };
    }

template<typename Database>
    constexpr auto  dynamic_insert(const Database&)
    -> decltype(blank_insert_t<Database>())
    {
        static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
        return { blank_insert_t<Database>() };
    }

template<typename Database, typename Table>
    constexpr auto  dynamic_insert_into(const Database&, Table table)
    -> decltype(blank_insert_t<Database>().into(table))
    {
        static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
        return { blank_insert_t<Database>().into(table) };
    }

}

template<typename Context>
    struct serializer_t<Context, postgresql::insert_name_t>
    {
        using _serialize_check = consistent_t;
        using T = postgresql::insert_name_t;

        static Context& _(const T& t, Context& context)
        {
            context << "INSERT ";
            return context;
        }
    };

}
