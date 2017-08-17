/*
 * Copyright (c) 2015, Volker Assmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef SQLPP_DYNAMIC_LOADING

#include "sqlpp11/postgresql/dynamic_libpq.h"
#include "sqlpp11/exception.h"

#include <atomic>
#include <cassert>
#include <string>
#include <mutex>

#if defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>
#else
#include <Windows.h>
#endif

namespace sqlpp {
namespace postgresql {
namespace dynamic {

#define DYNDEFINE(NAME) decltype( ::NAME ) * NAME


#if defined(__linux__) || defined(__APPLE__)
#define DYNLOAD(HNDL, NAME) NAME = reinterpret_cast<decltype( NAME )>( dlsym(HNDL, #NAME) )
#else
#define DYNLOAD(HNDL, NAME) NAME = reinterpret_cast<decltype( NAME )>(GetProcAddress(HNDL, #NAME))
#endif

DYNDEFINE(PQescapeStringConn);
DYNDEFINE(PQescapeString);
DYNDEFINE(PQescapeByteaConn);
DYNDEFINE(PQescapeBytea);
DYNDEFINE(PQfreemem);
DYNDEFINE(PQexec);
DYNDEFINE(PQprepare);
DYNDEFINE(PQexecPrepared);
DYNDEFINE(PQexecParams);
DYNDEFINE(PQresultStatus);
DYNDEFINE(PQresStatus);
DYNDEFINE(PQresultErrorMessage);
DYNDEFINE(PQresultErrorField);
DYNDEFINE(PQcmdTuples);
DYNDEFINE(PQcmdStatus);
DYNDEFINE(PQgetvalue);
DYNDEFINE(PQgetlength);
DYNDEFINE(PQgetisnull);
DYNDEFINE(PQoidValue);
DYNDEFINE(PQoidStatus);
DYNDEFINE(PQfformat);
DYNDEFINE(PQntuples);
DYNDEFINE(PQnfields);
DYNDEFINE(PQnparams);
DYNDEFINE(PQclear);
DYNDEFINE(PQfinish);
DYNDEFINE(PQstatus);
DYNDEFINE(PQconnectdb);
DYNDEFINE(PQerrorMessage);

#undef DYNDEFINE

#define STR(x) #x
#define GET_STR(x) STR(x)

#ifndef SQLPP_DYNAMIC_LOADING_FILENAME
#ifdef __linux__
#define SQLPP_DYNAMIC_LOADING_FILENAME libpq.so
#elif __APPLE__
#define SQLPP_DYNAMIC_LOADING_FILENAME libpq.dylib
#elif _WIN32
#define SQLPP_DYNAMIC_LOADING_FILENAME libpq.dll
#endif
#endif

void init_pg(std::string libname)
{
   std::atomic<bool> initialized{false};

   if (initialized) return;

   if (libname.empty())
   {
       libname = GET_STR(SQLPP_DYNAMIC_LOADING_FILENAME);
   }

#undef GET_STR
#undef STR

#if defined(__linux__) || defined(__APPLE__)
   void* handle = nullptr;
   handle = dlopen(libname.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#else
    HINSTANCE handle = nullptr;
    handle = LoadLibrary(libname.c_str());
#endif

   if (!handle) {
#if defined(__linux__) || defined(__APPLE__)
      throw sqlpp::exception(std::string("Could not load lib: ").append(dlerror()));
#elif defined(_WIN32)
       if (GetLastError() == 193)
       {
           throw sqlpp::exception("Could not load libpq (PostgreSQL) library - error code indicates you are mixing 32/64 bit DLLs"
                                  " (lib: " + libname + ")");
       } else {
           throw sqlpp::exception("Could not load libpq (PostgreSQL) library using LoadLibrary() (" + libname + ")");
       }
#endif
   }

   DYNLOAD(handle, PQescapeStringConn);
   DYNLOAD(handle, PQescapeString);
   DYNLOAD(handle, PQescapeByteaConn);
   DYNLOAD(handle, PQescapeBytea);
   DYNLOAD(handle, PQfreemem);
   DYNLOAD(handle, PQexec);
   DYNLOAD(handle, PQprepare);
   DYNLOAD(handle, PQexecPrepared);
   DYNLOAD(handle, PQexecParams);
   DYNLOAD(handle, PQresStatus);
   DYNLOAD(handle, PQresultStatus);
   DYNLOAD(handle, PQresultErrorMessage);
   DYNLOAD(handle, PQresultErrorField);
   DYNLOAD(handle, PQcmdStatus);
   DYNLOAD(handle, PQcmdTuples);
   DYNLOAD(handle, PQgetvalue);
   DYNLOAD(handle, PQgetlength);
   DYNLOAD(handle, PQgetisnull);
   DYNLOAD(handle, PQoidStatus);
   DYNLOAD(handle, PQoidValue);
   DYNLOAD(handle, PQfformat);
   DYNLOAD(handle, PQntuples);
   DYNLOAD(handle, PQnfields);
   DYNLOAD(handle, PQnparams);
   DYNLOAD(handle, PQclear);
   DYNLOAD(handle, PQfinish);
   DYNLOAD(handle, PQconnectdb);
   DYNLOAD(handle, PQstatus);
   DYNLOAD(handle, PQerrorMessage);

   if (PQescapeStringConn == nullptr || PQexec == nullptr)
   {
       throw sqlpp::exception("Initializing dynamically loaded SQLite3 functions failed");
   }
   initialized.store(true);
}

}
}
}

#undef DYNLOAD
#endif
