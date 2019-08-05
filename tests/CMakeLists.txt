# Copyright (c) 2015, Matthijs Möhlmann
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

# The available tests
set(test_names
	BasicTest
	ConstructorTest
	DateTest
	DateTime
	Exceptions
	Returning
	Select
	SelectTest
	TransactionTest
	TypeTest
	InsertOnConflict
	)

foreach(test_name ${test_names})
  set(test_names_src ${test_names_src} ${test_name}.cpp)
endforeach()

create_test_sourcelist(test_sources test_main.cpp ${test_names_src})
add_executable(sqlpp11-connector-postgresql_tests ${test_sources})
target_link_libraries(sqlpp11-connector-postgresql_tests PRIVATE sqlpp11 sqlpp11-connector-postgresql ${PostgreSQL_LIBRARIES})
target_include_directories(sqlpp11-connector-postgresql_tests PRIVATE ${sqlpp11_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIRS} )
target_compile_features(sqlpp11-connector-postgresql_tests PRIVATE cxx_auto_type)

foreach(test IN LISTS test_names)
	add_test(NAME sqlpp11-connector-postgresql.${test}
		COMMAND sqlpp11-connector-postgresql_tests ${test})
endforeach()

function(TestStaticCheck TEST_NAME)
	add_executable(${TEST_NAME} EXCLUDE_FROM_ALL static_fail/${TEST_NAME}.cpp)
	target_include_directories(${TEST_NAME} PRIVATE ${sqlpp11_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIRS} )
	target_link_libraries(${TEST_NAME} PRIVATE sqlpp11 sqlpp11-connector-postgresql ${PostgreSQL_LIBRARIES})
	add_test(NAME ${TEST_NAME}
		COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${TEST_NAME})
	set_tests_properties(${TEST_NAME} PROPERTIES WILL_FAIL TRUE)
endfunction()

# Add static_assert checks
TestStaticCheck(OnConflictInvalidParameter)
TestStaticCheck(OnConflictMissingAction)
TestStaticCheck(OnConflictMissingParameterDoUpdate)
TestStaticCheck(OnConflictMissingAssignmentsDoUpdate)
TestStaticCheck(OnConflictInvalidAssignmentsDoUpdate)
TestStaticCheck(OnConflictEmptyWhereDoUpdate)
TestStaticCheck(OnConflictInvalidWhereDoUpdate)
TestStaticCheck(ReturningEmptyAssert)
TestStaticCheck(ReturningInvalidArgument)

#macro (build_and_run arg)
#    # Add headers to sources to enable file browsing in IDEs
#    add_executable(Sqlpp11PostgreSQL${arg} ${arg}.cpp ${sqlpp_headers})
#    target_link_libraries(Sqlpp11PostgreSQL${arg} sqlpp11-connector-postgresql ${PostgreSQL_LIBRARIES})
#    include_directories("../include")
#    target_compile_features(Sqlpp11PostgreSQL${arg} PRIVATE cxx_auto_type)
#	target_include_directories(Sqlpp11PostgreSQL${arg} PRIVATE ${PostgreSQL_INCLUDE_DIRS} )
#    add_test(${arg} Sqlpp11PostgreSQL${arg})
#endmacro ()
#
#macro (build_and_run_dynamic arg)
#    add_executable(Sqlpp11PostgreSQL${arg}-dynamic ${arg}.cpp ${sqlpp_headers})
#    target_link_libraries(Sqlpp11PostgreSQL${arg}-dynamic sqlpp11-connector-postgresql-dynamic dl)
#    include_directories("../include")
#    target_compile_features(Sqlpp11PostgreSQL${arg}-dynamic PRIVATE cxx_auto_type)
#	target_include_directories(Sqlpp11PostgreSQL${arg}-dynamic PRIVATE ${PostgreSQL_INCLUDE_DIRS} )
#    add_test(${arg}-dyanmic Sqlpp11PostgreSQL${arg}-dynamic)
#endmacro()
#
#build_and_run(BasicTest)
##build_and_run_dynamic(BasicTest)
#build_and_run(DateTest)
#build_and_run(TypeTest)
#build_and_run(TransactionTest)
#build_and_run(ConstructorTest)
