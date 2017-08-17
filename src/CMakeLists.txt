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



set(
    source_path
    sqlpp11/postgresql
)


# Keep this in sync with the releases in sqlpp11
set(sqlpp-postgresql_VERSION_MAJOR 0)
set(sqlpp-postgresql_VERSION_MINOR 48)
set(sqlpp-postgresql_VERSION_PATCH 0)
set(sqlpp-postgresql_VERSION_STRING ${sqlpp-postgresql_VERSION_MAJOR}.${sqlpp-postgresql_VERSION_MINOR}.${sqlpp-postgresql_VERSION_PATCH})


############################################################################################################
#
#
function(
        sqlpp11_postgresql_target_emplace
        target
        sources
    )

    add_library(
            ${target}
        SHARED
            ${sources}
        )

    set_target_properties(
            ${target}
        PROPERTIES
            VERSION         ${sqlpp-postgresql_VERSION_STRING}
            SOVERSION       ${sqlpp-postgresql_VERSION_MAJOR}
            CXX_STANDARD    11
            DEBUG_POSTFIX   d
        )

    target_compile_features(
            ${target}
        PUBLIC
            cxx_auto_type
        )

    target_include_directories(
            ${target}
        PRIVATE
            "${PostgreSQL_INCLUDE_DIRS}"
            "${CMAKE_CURRENT_SOURCE_DIR}"
        INTERFACE
            $<INSTALL_INTERFACE:include>
        )


    target_compile_definitions(
            ${target}
        PRIVATE
            BUILDING_DLL
        )

    #
    #   specify rules for installing the target
    #
    install(
        TARGETS
            ${target}
        EXPORT
            ${SQLPP_EXPORT_NAME}
        RUNTIME # shared library
            DESTINATION
                bin
        ARCHIVE # import library
            DESTINATION
                lib
        LIBRARY # static library
            DESTINATION
                lib
        INCLUDES DESTINATION
            include
        )

endfunction()


################################################################################
#   collect sources
################################################################################

set(
    sources_cpp
        ${source_path}/bind_result.cpp
        ${source_path}/connection.cpp
        ${source_path}/connection_handle.cpp
        ${source_path}/dynamic_libpq.cpp
        ${source_path}/exception.cpp
        ${source_path}/prepared_statement.cpp
        ${source_path}/prepared_statement_handle.cpp
        ${source_path}/result.cpp
    )

set(
    sources_h
        ${source_path}/bind_result.h
        ${source_path}/connection.h
        ${source_path}/connection_config.h
        ${source_path}/connection_handle.h
        ${source_path}/dynamic_libpq.h
        ${source_path}/exception.h
        ${source_path}/insert.h
        ${source_path}/postgresql.h
        ${source_path}/prepared_statement.h
        ${source_path}/prepared_statement_handle.h
        ${source_path}/result.h
        ${source_path}/returning.h
        ${source_path}/returning_column_list.h
        ${source_path}/serializer.h
        ${source_path}/update.h
        ${source_path}/visibility.h
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

sqlpp11_postgresql_target_emplace(
    ${targetA}
    "${sources}"
    )

sqlpp11_postgresql_target_emplace(
    ${targetB}
    "${sources}"
    )


# individual settings TargetA
target_link_libraries(
        ${targetA}
    PRIVATE
        ${PostgreSQL_LIBRARIES}
    INTERFACE

    )

# individual settings TargetB
target_compile_definitions(
        ${targetB}
    PRIVATE
        SQLPP_DYNAMIC_LOADING
        SQLPP_DYNAMIC_LOADING_FILENAME=${POSTGRESQL_LIB_FILE}
    INTERFACE
        SQLPP_DYNAMIC_LOADING
    )


################################################################################
#   Config-file Package (Targets-File)
################################################################################

# actually generate the export file (into the build dir)
install(
    EXPORT
        ${SQLPP_EXPORT_NAME}
    DESTINATION
        lib/cmake/${SQLPP_EXPORT_NAME}
    NAMESPACE
        ${SQLPP_NAMESAPCE}
    FILE
        Sqlpp11PostgreSQLConfig.cmake
    )





# and install it
#install(
#    FILES
#        "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlConfigVersion.cmake"
#    DESTINATION
#        ${ConfigPackageLocation}
#    COMPONENT
#        Devel
#    )


################################################################################
#   Config-file Package (Version-File)
################################################################################


#include(CMakePackageConfigHelpers)
#
#write_basic_package_version_file(
#        "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlConfigVersion.cmake"
#    VERSION
#        ${Upstream_VERSION}
#    COMPATIBILITY
#        AnyNewerVersion
#)
#
#export(
#    EXPORT
#        ${SQLPP_EXPORT_NAME}
#    FILE
#        "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlTargets.cmake"
#    NAMESPACE
#        ${SQLPP_NAMESAPCE}
#)
#

################################################################################
#   install
################################################################################

# header files
install(
    FILES
        ${sources_h}
    DESTINATION
        include/${source_path}
    )



#install(
#    FILES
#        cmake/Sqlpp-postgresqlConfig.cmake
#    DESTINATION
#        ${ConfigPackageLocation}
#    COMPONENT
#        Devel
#    )
