# Copyright (c) 2015, Matthijs Möhlmann
# Copyright (c) 2016, Christian Dávid
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#   Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


set(LIB_HEADERS
    detail/prepared_statement_handle.h
)


################################################################################
#
################################################################################
if (WIN32)

    get_filename_component(
        POSTGRESQL_LIB_FILE
        ${PostgreSQL_LIBRARIES}
        NAME
        )

elseif(APPLE)

    set(POSTGRESQL_LIB_FILE "lib${PostgreSQL_LIBRARIES}.dylib")

else()

    set(POSTGRESQL_LIB_FILE "lib${PostgreSQL_LIBRARIES}.so")

endif()

message("PostgreSQL LIB FILE: ${POSTGRESQL_LIB_FILE}")



################################################################################
#   collect sources
################################################################################



set(
    sources_cpp
        bind_result.cpp
        connection.cpp
        exception.cpp
        prepared_statement.cpp
        detail/connection_handle.cpp
        detail/prepared_statement_handle.cpp
        result.cpp
    )

set(
    sources_h
        ../include/sqlpp11/postgresql/bind_result.h
        ../include/sqlpp11/postgresql/connection.h
        ../include/sqlpp11/postgresql/connection_config.h
        ../include/sqlpp11/postgresql/dynamic_libpq.h
        ../include/sqlpp11/postgresql/exception.h
        ../include/sqlpp11/postgresql/insert.h
        ../include/sqlpp11/postgresql/postgresql.h
        ../include/sqlpp11/postgresql/prepared_statement.h
        ../include/sqlpp11/postgresql/result.h
        ../include/sqlpp11/postgresql/returning.h
        ../include/sqlpp11/postgresql/returning_column_list.h
        ../include/sqlpp11/postgresql/serializer.h
        ../include/sqlpp11/postgresql/update.h
        ../include/sqlpp11/postgresql/visibility.h
    )

set(
        sources
        ${sources_h}
        ${sources_cpp}
    )

source_group(
        h
    FILES
        ${sources_h}
    )

source_group(
        cpp
    FILES
        ${sources_cpp}
    )


################################################################################
#   register targets
################################################################################
set(
        targetA
        sqlpp-postgresql
    )

set(
        targetB
        sqlpp-postgresql-dynamic
    )


add_library(
        ${targetA}
    SHARED
        ${sources}
)

add_library(
        ${targetB}
    SHARED
        ${sources}
        detail/dynamic_libpq.cpp
    )

################################################################################
set_property(
    TARGET
        ${targetA}
    PROPERTY
        CXX_STANDARD 11
    )

target_compile_options(
        ${targetA}
    PRIVATE
        -DBUILDING_DLL
    )

target_compile_options(
        ${targetB}
    INTERFACE
        -DSQLPP_DYNAMIC_LOADING
    )

target_compile_options(
        ${targetB}
    PRIVATE
        -DBUILDING_DLL
        -DSQLPP_DYNAMIC_LOADING
        -DSQLPP_DYNAMIC_LOADING_FILENAME=${POSTGRESQL_LIB_FILE}
    )

target_compile_features(
        ${targetA}
    PRIVATE
        cxx_auto_type
    )

target_compile_features(
        ${targetB}
    PRIVATE
        cxx_auto_type
    )

target_link_libraries(
        ${targetA}
    PRIVATE
        ${PostgreSQL_LIBRARIES}
    )

target_include_directories(
        ${targetA}
    PRIVATE
        ${PostgreSQL_INCLUDE_DIRS}
        "../include"
    )

target_include_directories(
        ${targetB}
    PRIVATE
        ${PostgreSQL_INCLUDE_DIRS}
        "../include"
    )




# The include directory should be PUBLIC. However, that would store an absolute path in the package configuration
# so leave it PRIVATE and correct that in the ...Config.cmake file. This can be changed when the FindPostgreSQL.cmake got updated.
#target_include_directories(sqlpp-postgresql PUBLIC ${PostgreSQL_INCLUDE_DIRS})

# Keep this in sync with the releases in sqlpp11
set(sqlpp-postgresql_VERSION_MAJOR 0)
set(sqlpp-postgresql_VERSION_MINOR 48)
set(sqlpp-postgresql_VERSION_PATCH 0)
set(sqlpp-postgresql_VERSION_STRING ${sqlpp-postgresql_VERSION_MAJOR}.${sqlpp-postgresql_VERSION_MINOR}.${sqlpp-postgresql_VERSION_PATCH})

set_target_properties(
        ${targetA}
    PROPERTIES
        VERSION     ${sqlpp-postgresql_VERSION_STRING}
        SOVERSION   ${sqlpp-postgresql_VERSION_MAJOR}
    )

set_target_properties(
        ${targetB}
    PROPERTIES
        VERSION     ${sqlpp-postgresql_VERSION_STRING}
        SOVERSION   ${sqlpp-postgresql_VERSION_MAJOR}
    )

install(
    TARGETS
        ${targetA}
    EXPORT
        postgresqlTargets
    DESTINATION
        lib
    )

install(
    TARGETS
        ${targetB}
    EXPORT
        postgresqlTargets
    DESTINATION
        lib
    )

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlConfigVersion.cmake"
    VERSION
        ${Upstream_VERSION}
    COMPATIBILITY
        AnyNewerVersion
)

export(
    EXPORT
        postgresqlTargets
    FILE
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlTargets.cmake"
    NAMESPACE
        Sqlpp::
)

install(
    EXPORT
        postgresqlTargets
    FILE
        Sqlpp-postgresqlTargets.cmake
    NAMESPACE
        Sqlpp::
    DESTINATION
        ${ConfigPackageLocation}
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlConfigVersion.cmake"
    DESTINATION
        ${ConfigPackageLocation}
    COMPONENT
        Devel
)
