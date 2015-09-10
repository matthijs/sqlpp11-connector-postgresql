#pragma once
#include "postgresql/libpq-fe.h"
#include <sstream>
#include <string>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <sqlpp11/postgresql/pgexception.h>

namespace sqlpp{
namespace postgresql {

using std::string;

class __attribute__((__visibility__("default"))) Result{
public:
    Result();
    Result( PGresult *res );

    ExecStatusType status();


    void operator = ( PGresult *res );
    operator bool () const {
        return m_result == nullptr ? false : true;
    }

    void clear(){
        if(m_result)
            PQclear(m_result);
        m_result = nullptr;
    }

    size_t affected_rows();

    size_t records_size() const{
        return PQntuples(m_result);
    }

    size_t field_count() const {
        return PQnfields(m_result);
    }

    bool isNull(size_t record, size_t field) const {
        return PQgetisnull(m_result, record, field);
    }

    template<typename T>
    inline T getValue(size_t record, size_t field) const {
        checkIndex(record, field);
        static_assert(std::is_arithmetic<T>::value, "Value must be numeric type");
        T t(0);
        try{
            t = boost::lexical_cast<T>(PQgetvalue(m_result, record, field));
        }
        catch(boost::bad_lexical_cast){}
        return t;
    }

    size_t length(size_t record, size_t field) const {
        return PQgetlength(m_result, record, field);
    }

    ~Result(){
        clear();
    }

    PGresult *get(){
        return m_result;
    }

private:
    bool hasError();

    void checkIndex(size_t record, size_t field) const throw(std::out_of_range) {
        if(record > records_size() || field > field_count() )
            throw std::out_of_range("libpq error: index out of range");
    }

    PGresult *m_result;
};

template<>
inline const char * Result::getValue<const char *>(size_t record, size_t field) const {
    return const_cast<const char *>(PQgetvalue(m_result, record, field));
}

template<>
inline bool Result::getValue<bool>(size_t record, size_t field) const {
    checkIndex(record, field);
    auto val = PQgetvalue(m_result, record, field);
    if( *val == 't' )
        return true;
    else if( *val == 'f' )
        return false;
    return const_cast<const char *>(val);
}

}
}
