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

add_library(sqlpp-postgresql SHARED
	bind_result.cpp
	connection.cpp
	prepared_statement.cpp
	detail/connection_handle.cpp)

add_library(sqlpp-postgresql-dynamic SHARED
        bind_result.cpp
        connection.cpp
        prepared_statement.cpp
        detail/connection_handle.cpp
        detail/dynamic_libpq.cpp)

if (WIN32)
get_filename_component(POSTGRESQL_LIB_FILE ${PostgreSQL_LIBRARIES} NAME)
elseif(APPLE)
set(POSTGRESQL_LIB_FILE "lib${PostgreSQL_LIBRARIES}.dylib")
else()
set(POSTGRESQL_LIB_FILE "lib${PostgreSQL_LIBRARIES}.so")
endif()

message("PostgreSQL LIB FILE: ${POSTGRESQL_LIB_FILE}")
target_compile_options(sqlpp-postgresql-dynamic INTERFACE -DSQLPP_DYNAMIC_LOADING)
target_compile_options(sqlpp-postgresql-dynamic PRIVATE -DSQLPP_DYNAMIC_LOADING -DSQLPP_DYNAMIC_LOADING_FILENAME=${POSTGRESQL_LIB_FILE})
target_compile_features(sqlpp-postgresql PRIVATE cxx_auto_type)
target_compile_features(sqlpp-postgresql-dynamic PRIVATE cxx_auto_type)

target_link_libraries(sqlpp-postgresql PRIVATE ${PostgreSQL_LIBRARIES})

target_include_directories(sqlpp-postgresql PRIVATE ${PostgreSQL_INCLUDE_DIRS} "../include/")
target_include_directories(sqlpp-postgresql-dynamic PRIVATE ${PostgreSQL_INCLUDE_DIRS} "../include/")

# The include directory should be PUBLIC. However, that would store an absolute path in the package configuration
# so leave it PRIVATE and correct that in the ...Config.cmake file. This can be changed when the FindPostgreSQL.cmake got updated.
#target_include_directories(sqlpp-postgresql PUBLIC ${PostgreSQL_INCLUDE_DIRS})

# Keep this in sync with the releases in sqlpp11
set(sqlpp-postgresql_VERSION_MAJOR 0)
set(sqlpp-postgresql_VERSION_MINOR 48)
set(sqlpp-postgresql_VERSION_PATCH 0)
set(sqlpp-postgresql_VERSION_STRING ${sqlpp-postgresql_VERSION_MAJOR}.${sqlpp-postgresql_VERSION_MINOR}.${sqlpp-postgresql_VERSION_PATCH})
set_target_properties(sqlpp-postgresql PROPERTIES VERSION ${sqlpp-postgresql_VERSION_STRING}
	        SOVERSION ${sqlpp-postgresql_VERSION_MAJOR})
set_target_properties(sqlpp-postgresql-dynamic PROPERTIES VERSION ${sqlpp-postgresql_VERSION_STRING}
            SOVERSION ${sqlpp-postgresql_VERSION_MAJOR})

install(TARGETS sqlpp-postgresql
        EXPORT postgresqlTargets
        DESTINATION lib)
install(TARGETS sqlpp-postgresql-dynamic
        EXPORT postgresqlTargets
        DESTINATION lib)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlConfigVersion.cmake"
  VERSION ${Upstream_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT postgresqlTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlTargets.cmake"
  NAMESPACE Sqlpp::
)

install(EXPORT postgresqlTargets
  FILE Sqlpp-postgresqlTargets.cmake
  NAMESPACE Sqlpp::
  DESTINATION ${ConfigPackageLocation}
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-postgresqlConfigVersion.cmake"
  DESTINATION ${ConfigPackageLocation}
  COMPONENT Devel
)
