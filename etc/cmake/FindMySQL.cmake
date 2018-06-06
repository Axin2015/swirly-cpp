# The Restful Matching-Engine.
# Copyright (C) 2013, 2018 Swirly Cloud Limited.
#
# This program is free software; you can redistribute it and/or modify it under the terms of the
# GNU General Public License as published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program; if
# not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

# - Find MySQL
# Find the native MySQL headers and libraries.
#
#  MYSQL_INCLUDE_DIRS - Where to find mysql.h, etc.
#  MYSQL_LIBRARIES    - List of MySQL libraries.
#  MYSQL_FOUND        - True if MySQL found.

# Look for the header file.
FIND_PATH(MYSQL_INCLUDE_DIR NAMES mysql.h
  HINTS "$ENV{MYSQL_HOME}/include"
  PATHS
  "/usr/include/mysql"
  "/usr/local/include/mysql")

# Look for the library.
FIND_LIBRARY(MYSQL_LIBRARY NAMES mysqlclient
  HINTS "$ENV{MYSQL_HOME}/lib"
  PATHS
  "/usr/lib/mysql"
  "/usr/lib64/mysql"
  "/usr/local/lib/mysql"
  "/usr/local/lib64/mysql")

# Look for the program.
FIND_PROGRAM(MYSQL_EXECUTABLE NAMES mysql_config
  HINTS "$ENV{MYSQL_HOME}/bin")

IF(MYSQL_INCLUDE_DIR)
  FILE(STRINGS "${MYSQL_INCLUDE_DIR}/mysql_version.h" _mysql_version_raw
    REGEX "^#define[\t ]+MYSQL_SERVER_VERSION[\t ]+\"[0-9]+(\\.[0-9]+)+.*\"")
  STRING(REGEX REPLACE "^#define[\t ]+MYSQL_SERVER_VERSION[\t ]+\"([0-9]+(\\.[0-9]+)+).*\"" "\\1"
    MYSQL_VERSION_STRING "${_mysql_version_raw}")
  UNSET(_mysql_version_raw)
ENDIF()

# Handle the QUIETLY and REQUIRED arguments and set MYSQL_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MySQL
  FOUND_VAR     MYSQL_FOUND
  REQUIRED_VARS MYSQL_INCLUDE_DIR
  MYSQL_LIBRARY
  MYSQL_EXECUTABLE
  VERSION_VAR   MYSQL_VERSION_STRING
  FAIL_MESSAGE  "Failed to find MySQL")

MARK_AS_ADVANCED(MYSQL_INCLUDE_DIR MYSQL_LIBRARY MYSQL_EXECUTABLE)

# Copy the results to the output variables.
IF(MYSQL_FOUND)
  SET(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
  SET(MYSQL_LIBRARIES ${MYSQL_LIBRARY})
ENDIF()
