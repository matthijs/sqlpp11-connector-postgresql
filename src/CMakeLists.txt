# Distributed under the BSD 2-Clause License.
# See accompanying license file for details.


############################################################################################################
#   function
#       sqlpp11postgresql_target_emplace
#
#   Create a new target and set some default properties.
#
#   parameter
#       target:  The name of the new target.
#       sources: The list of source files.
#
function(
        sqlpp11postgresql_target_emplace
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
            VERSION         ${SQLPPP_VERSION_STRING}
            SOVERSION       ${SQLPPP_VERSION_MAJOR}
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

    target_link_libraries(
            ${target}
        PUBLIC
            sqlpp11
        )

    #
    #   specify rules for installing the target
    #
    install(
        TARGETS
            ${target}
        EXPORT
            ${SQLPPP_EXPORT_NAME}
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
    source_path
    sqlpp11/postgresql
)

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
    ${SQLPPP_EXPORT_NAME}
    )

set(
    targetB
    ${SQLPPP_EXPORT_NAME}_dynamic
    )

sqlpp11postgresql_target_emplace(
    ${targetA}
    "${sources}"
    )

sqlpp11postgresql_target_emplace(
    ${targetB}
    "${sources}"
    )


# individual settings TargetA
target_link_libraries(
        ${targetA}
    PRIVATE
        ${PostgreSQL_LIBRARIES}
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
#   Package: Target-file
################################################################################

# generate the exports file into the installation folder
install(
    EXPORT
        ${SQLPPP_EXPORT_NAME}
    DESTINATION
        cmake
    NAMESPACE
        ${SQLPPP_NAMESAPCE}
    FILE
        ${SQLPPP_EXPORT_NAME}Targets.cmake
    )

################################################################################
#   Package: Config-file
################################################################################

install(
    FILES
        cmake/${SQLPPP_EXPORT_NAME}Config.cmake
    DESTINATION
        cmake
    )

################################################################################
#   Config-file Package (Version-File)
################################################################################

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${SQLPPP_EXPORT_NAME}Version.cmake"
    VERSION
        ${Upstream_VERSION}
    COMPATIBILITY
        AnyNewerVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${SQLPPP_EXPORT_NAME}Version.cmake"
    DESTINATION
        cmake
)

################################################################################
#   Installation Package
################################################################################

# header files
install(
    FILES
        ${sources_h}
    DESTINATION
        include/${source_path}
    )
