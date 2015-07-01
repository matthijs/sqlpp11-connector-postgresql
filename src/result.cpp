#include "sqlpp11/postgresql/result.h"
#include "string"

std::string errmsg = "PostgreSQL error: ";

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
}

ExecStatusType Result::status(){
    return PQresultStatus(m_result);
}

string Result::errorStr(){
    return errmsg + string(PQresStatus(status()))+ string(": ")+ string(PQresultErrorMessage(m_result));
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

void Result::operator =(PGresult *res){
    m_result = res;
}

Result::~Result(){
    PQclear(m_result);
}

}
}
