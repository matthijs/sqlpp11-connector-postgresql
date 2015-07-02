#pragma once
#include "postgresql/libpq-fe.h"
#include <sstream>
#include "string"

#include <boost/lexical_cast.hpp>

namespace sqlpp{
namespace postgresql {

using std::string;

class __attribute__((__visibility__("default"))) Result{
public:
    Result();
    Result( PGresult *res );

    ExecStatusType status();
    bool hasError();
    string errorStr();

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

    size_t records_size();

    size_t field_count();

    bool isNull(size_t record, size_t field) const {
        return PQgetisnull(m_result, record, field);
    }

    template<typename T>
    inline T getValue(size_t record, size_t field) const {
        return boost::lexical_cast<T>(PQgetvalue(m_result, record, field));
    }

    size_t length(size_t record, size_t field) const {
        return PQgetlength(m_result, record, field);
    }

    ~Result();

    PGresult *get(){
        return m_result;
    }

private:
    PGresult *m_result;
};

template<>
inline const char * Result::getValue<const char *>(size_t record, size_t field) const {
    return const_cast<const char *>(PQgetvalue(m_result, record, field));
}

}
}
