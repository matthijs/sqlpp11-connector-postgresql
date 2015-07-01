#pragma once
#include "postgresql/libpq-fe.h"
#include <sstream>
#include "string"

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

    size_t size(){
        std::istringstream in(PQcmdTuples(m_result));
        size_t result;
        in >> result;
        return result;
    }
    ~Result();

private:
    PGresult *m_result;
};
}
}
