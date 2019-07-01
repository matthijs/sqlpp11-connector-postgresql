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

add_library(sqlpp11-connector-postgresql STATIC
	bind_result.cpp
	connection.cpp
	exception.cpp
	prepared_statement.cpp
	detail/connection_handle.cpp
	detail/prepared_statement_handle.cpp
	result.cpp
)

add_library(sqlpp11-connector-postgresql-dynamic SHARED
	bind_result.cpp
	connection.cpp
	exception.cpp
	prepared_statement.cpp
	detail/connection_handle.cpp
	detail/prepared_statement_handle.cpp
	detail/dynamic_libpq.cpp
	result.cpp
)

if (WIN32)
	get_filename_component(POSTGRESQL_LIB_FILE ${PostgreSQL_LIBRARIES} NAME)
elseif(APPLE)
	set(POSTGRESQL_LIB_FILE "lib${PostgreSQL_LIBRARIES}.dylib")
else()
    if("${PostgreSQL_LIBRARIES}" MATCHES ".+lib.+so")
	    get_filename_component(POSTGRESQL_LIB_FILE ${PostgreSQL_LIBRARIES} NAME)
    else()
        set(POSTGRESQL_LIB_FILE "${PostgreSQL_LIBRARIES}.so")
    endif()
endif()

# Generate code coverage if enabled
if (CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	# Add required flags for code coverage
	target_compile_options(sqlpp11-connector-postgresql PUBLIC -O0 --coverage)
	target_compile_options(sqlpp11-connector-postgresql-dynamic PUBLIC -O0 --coverage)
	if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
		target_link_options(sqlpp11-connector-postgresql INTERFACE --coverage)
		target_link_options(sqlpp11-connector-postgresql-dynamic INTERFACE --coverage)
	else()
		target_link_libraries(sqlpp11-connector-postgresql INTERFACE --coverage)
		target_link_libraries(sqlpp11-connector-postgresql-dynamic INTERFACE --coverage)
	endif()
endif()

target_compile_options(sqlpp11-connector-postgresql-dynamic INTERFACE -DSQLPP_DYNAMIC_LOADING)
target_compile_options(sqlpp11-connector-postgresql-dynamic PRIVATE -DBUILDING_DLL -DSQLPP_DYNAMIC_LOADING -DSQLPP_DYNAMIC_LOADING_FILENAME=${POSTGRESQL_LIB_FILE})
target_compile_features(sqlpp11-connector-postgresql PRIVATE cxx_auto_type)
target_compile_features(sqlpp11-connector-postgresql-dynamic PRIVATE cxx_auto_type)

target_link_libraries(sqlpp11-connector-postgresql PRIVATE sqlpp11 $<BUILD_INTERFACE:${PostgreSQL_LIBRARIES}>)
target_link_libraries(sqlpp11-connector-postgresql-dynamic PUBLIC sqlpp11 PRIVATE ${PostgreSQL_LIBRARIES})

target_include_directories(sqlpp11-connector-postgresql PRIVATE ${sqlpp11_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIRS} "../include/")
target_include_directories(sqlpp11-connector-postgresql-dynamic PRIVATE ${sqlpp11_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIRS} "../include/")

# The include directory should be PUBLIC. However, that would store an absolute path in the package configuration
# so leave it PRIVATE and correct that in the ...Config.cmake file. This can be changed when the FindPostgreSQL.cmake got updated.
#target_include_directories(sqlpp11-connector-postgresql PUBLIC ${PostgreSQL_INCLUDE_DIRS})

# Keep this in sync with the releases in sqlpp11
set(sqlpp11-connector-postgresql_VERSION_MAJOR 0)
set(sqlpp11-connector-postgresql_VERSION_MINOR 54)
set(sqlpp11-connector-postgresql_VERSION_PATCH 0)
set(sqlpp11-connector-postgresql_VERSION_STRING ${sqlpp11-connector-postgresql_VERSION_MAJOR}.${sqlpp11-connector-postgresql_VERSION_MINOR}.${sqlpp11-connector-postgresql_VERSION_PATCH})
set_target_properties(sqlpp11-connector-postgresql PROPERTIES VERSION ${sqlpp11-connector-postgresql_VERSION_STRING}
	        SOVERSION ${sqlpp11-connector-postgresql_VERSION_MAJOR})
set_target_properties(sqlpp11-connector-postgresql-dynamic PROPERTIES VERSION ${sqlpp11-connector-postgresql_VERSION_STRING}
            SOVERSION ${sqlpp11-connector-postgresql_VERSION_MAJOR})

set_property(TARGET sqlpp11-connector-postgresql APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

set_property(TARGET sqlpp11-connector-postgresql-dynamic APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

install(TARGETS sqlpp11-connector-postgresql
        EXPORT postgresqlTargets
        DESTINATION lib)
install(TARGETS sqlpp11-connector-postgresql-dynamic
        EXPORT postgresqlTargets
        DESTINATION lib)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-connector-postgresqlConfigVersion.cmake"
  VERSION ${Upstream_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT postgresqlTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-connector-postgresqlTargets.cmake"
  NAMESPACE Sqlpp::
)

install(EXPORT postgresqlTargets
  FILE Sqlpp-connector-postgresqlTargets.cmake
  NAMESPACE Sqlpp::
  DESTINATION ${ConfigPackageLocation}
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp-connector-postgresqlConfigVersion.cmake"
  DESTINATION ${ConfigPackageLocation}
  COMPONENT Devel
)
