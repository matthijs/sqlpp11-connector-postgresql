#include <sqlpp11/sqlpp11.h>
#include "sqlpp11/postgresql/postgresql.h"

struct C_uid {
    struct _alias_t {
        static constexpr const char _literal[] ="c_uid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
            struct _member_t {
                T c_uid;
                T &operator()() { return c_uid; }
                const T &operator()() const { return c_uid; }
            };
    };

    using _traits = sqlpp::make_traits<sqlpp::integer>;
};

struct t_acl : sqlpp::table_t<t_acl,
        C_uid
        > {
    using _value_type = sqlpp::no_value_t;
    struct _alias_t {
        static constexpr const char _literal[] = "t_acl";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
            struct _member_t {
                T t_acl;
                T &operator()() { return t_acl; }
                const T &operator()() const { return t_acl; }
            };
    };
};


struct column1 {
    struct _alias_t {
        static constexpr const char _literal[] ="column1";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
            struct _member_t {
                T column1;
                T &operator()() { return column1; }
                const T &operator()() const { return column1; }
            };
    };

    using _traits = sqlpp::make_traits<sqlpp::integer>;
};

struct tab2: sqlpp::table_t<tab2,
        column1
        > {
    using _value_type = sqlpp::no_value_t;
    struct _alias_t {
        static constexpr const char _literal[] = "tab2";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
            struct _member_t {
                T tab2;
                T &operator()() { return tab2; }
                const T &operator()() const { return tab2; }
            };
    };
};


constexpr t_acl a;
constexpr tab2 t2;

int main(){
    std::shared_ptr<sqlpp::postgresql::connection_config>conf( new sqlpp::postgresql::connection_config );
    conf->host = "localhost";
    conf->dbname = "postagres";
    conf->user = "postgres";
    conf->password = "postgres";
    conf->debug = true;
    conf->port = 5432;

    try{
        sqlpp::postgresql::connection db ( conf );
        db.start_transaction();
        db(sqlpp::postgresql::insert_into(a).set(a.c_uid = 99999));
        auto sel = db(sqlpp::postgresql::insert_into(a).set(a.c_uid = 99999).returning(a.c_uid));

        std::cout << sel.front().c_uid;

        auto multi_insert = sqlpp::postgresql::insert_into(a).columns(a.c_uid).returning(a.c_uid);
        multi_insert.values.add(a.c_uid=1000001);
        multi_insert.values.add(a.c_uid=1000003);
        auto inserted = db(multi_insert);

        for(const auto &row: inserted )
            std::cout<< row.c_uid;

        db.rollback_transaction();
    }
    catch(sqlpp::postgresql::pg_exception e){
        std::cout << e.what();
    }
}
