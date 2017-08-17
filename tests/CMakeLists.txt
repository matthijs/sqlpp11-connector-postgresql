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

    target_link_libraries(
        ${target}
        sqlpp-postgresql
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
            "../src"
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
