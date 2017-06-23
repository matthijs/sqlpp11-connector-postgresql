# - Find sqlpp11
# Find the sqlpp11 includes
# This module defines
#  SQLPP11_INCLUDE_DIR, where to find sqlpp11.h header file
#  SQLPP11_FOUND, If false, do not try to use sqlpp11

find_path(SQLPP11_INCLUDE_DIR sqlpp11.h
	/usr/include/sqlpp11
	/usr/local/include/sqlpp11
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sqlpp11 DEFAULT_MSG SQLPP11_INCLUDE_DIR)

mark_as_advanced(SQLPP11_INCLUDE_DIR)
