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

add_library(sqlpp-postgresql SHARED
	bind_result.cpp
	connection.cpp
	prepared_statement.cpp
	detail/connection_handle.cpp)

# Keep this in sync with the releases in sqlpp11
set(sqlpp-postgresql_VERSION_MAJOR 0)
set(sqlpp-postgresql_VERSION_MINOR 34)
set(sqlpp-postgresql_VERSION_PATCH 0)
set(sqlpp-postgresql_VERSION_STRING ${cwf_VERSION_MAJOR}.${cwf_VERSION_MINOR}.${cwf_VERSION_PATCH})
set_target_properties(sqlpp-postgresql PROPERTIES VERSION ${sqlpp-postgresql_VERSION_STRING}
	        SOVERSION ${sqlpp-postgresql_VERSION_MAJOR})

install(TARGETS sqlpp-postgresql DESTINATION lib)
