# Distributed under the BSD 2-Clause License.
# See accompanying license file for details.

# Only because I haven't used older versions.
cmake_minimum_required(
    VERSION
        3.9
    FATAL_ERROR
    )


############################################################################################################
# Set a name, version, and enable languages for the entire project.
#   must be at the top:
#   \see https://stackoverflow.com/questions/571292/how-do-i-fix-an-apparently-corrupted-cmake-build
#   \see http://www.mail-archive.com/cmake@cmake.org/msg13392.html
############################################################################################################
project(
        sqlpp11postgresql
    VERSION
        0.2
    LANGUAGES
        CXX
    )



############################################################################################################
#   find packages
############################################################################################################
find_package(Sqlpp11    REQUIRED)
find_package(PostgreSQL REQUIRED)


############################################################################################################
#   global variables
############################################################################################################
set(
    SQLPPP_NAMESAPCE
    "Sqlpp11::"
    )

set(
    SQLPPP_EXPORT_NAME
    "sqlpp11postgresql"
    )

set(
    ConfigPackageLocation
    lib/cmake/${SQLPPP_EXPORT_NAME}
    )

# Keep this in sync with the releases in sqlpp11
set(
    SQLPPP_VERSION_MAJOR
    0
    )

set(
    SQLPPP_VERSION_MINOR
    48
    )

set(
    SQLPPP_VERSION_PATCH
    0
    )

set(
    SQLPPP_VERSION_STRING
    ${SQLPPP_VERSION_MAJOR}.${SQLPPP_VERSION_MINOR}.${SQLPPP_VERSION_PATCH}
    )




option(
    ENABLE_TESTS
    "Include Tests"
    ON
    )

#
#   Enable/Disable output of compile commands during generation.
#   If enabled, generates a compile_commands.json file containing the exact compiler calls.
#
set(
    CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    )


################################################################################
#   derermine the filename of the postgres shared library
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


################################################################################
#
################################################################################

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)

if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
