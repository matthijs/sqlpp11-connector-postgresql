# Distributed under the BSD 2-Clause License.
# See accompanying license file for details.


function(
    test_emplace
    testname
    )

    set(
        target
        Test_${testname}
        )

    # Add headers to sources to enable file browsing in IDEs
    add_executable(
        ${target}
        ${testname}.cpp
        )

    set_target_properties(
            ${target}
        PROPERTIES
            CXX_STANDARD    11
            DEBUG_POSTFIX   d
        )

    target_link_libraries(
        ${target}
        ${SQLPPP_EXPORT_NAME}
        ${PostgreSQL_LIBRARIES}
        )


    target_compile_features(
        ${target}
        PRIVATE
            cxx_auto_type
        )

    target_include_directories(
        ${target}
        PRIVATE
            ${PostgreSQL_INCLUDE_DIRS}
            "${CMAKE_CURRENT_SOURCE_DIR}/../src"
        )

    add_test(
        NAME
            ${testname}
        COMMAND
            ${target}
        )

endfunction()

#TODO(add tests for the dynamic target)

test_emplace(BasicTest)
test_emplace(DateTest)
test_emplace(TypeTest)
test_emplace(TransactionTest)
test_emplace(ConstructorTest)
