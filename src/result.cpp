#include "sqlpp11/postgresql/result.h"
#include <sqlpp11/postgresql/pgexception.h>
#include "string"
#include "postgresql/libpq-fe.h"

namespace sqlpp{
namespace postgresql{

using std::string;

Result::Result():
    m_result(nullptr)
{
}

Result::Result(PGresult *res):
    m_result(res)
{
    if(hasError())
        throw pg_exception( m_result );
}

ExecStatusType Result::status(){
    return PQresultStatus( m_result );
}

bool Result::hasError(){
    auto ret = status();
    switch(ret) {
    case PGRES_EMPTY_QUERY:
    case PGRES_COPY_OUT:
    case PGRES_COPY_IN:
    case PGRES_BAD_RESPONSE:
    case PGRES_NONFATAL_ERROR:
    case PGRES_FATAL_ERROR:
    case PGRES_COPY_BOTH:
        return true;
    case PGRES_COMMAND_OK:
    case PGRES_TUPLES_OK:
    case PGRES_SINGLE_TUPLE:
    default:
        return false;
    }
}

void Result::checkIndex(size_t record, size_t field) const throw(std::out_of_range) {
    if(record > records_size() || field > field_count() )
        throw std::out_of_range("libpq error: index out of range");
}

void Result::operator =(PGresult *res){
    m_result = res;
    if(hasError()){
        throw pg_exception( m_result );
    }
}

sqlpp::postgresql::Result::operator bool() const {
    return m_result == nullptr ? false : true;
}

void Result::clear(){
    if(m_result)
        PQclear(m_result);
    m_result = nullptr;
}

size_t Result::affected_rows(){
    size_t affected = 0;
    try{
        affected = boost::lexical_cast<size_t>( PQcmdTuples(m_result));
    }
    catch(boost::bad_lexical_cast){}

    return affected;
}

size_t Result::records_size() const{
    return PQntuples(m_result);
}

size_t Result::field_count() const {
    return PQnfields(m_result);
}

bool Result::isNull(size_t record, size_t field) const {
    return PQgetisnull(m_result, record, field);
}

size_t Result::length(size_t record, size_t field) const {
    return PQgetlength(m_result, record, field);
}

Result::~Result(){
    clear();
}

PGresult *Result::get(){
    return m_result;
}

}
}
