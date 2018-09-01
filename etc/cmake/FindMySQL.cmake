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
find_path(MYSQL_INCLUDE_DIR NAMES mysql.h
  HINTS "$ENV{MYSQL_HOME}/include"
  PATHS
  "/usr/include/mysql"
  "/usr/local/include/mysql")

# Look for the library.
find_library(MYSQL_LIBRARY NAMES mariadb mysqlclient
  HINTS "$ENV{MYSQL_HOME}/lib"
  PATHS
  "/usr/lib/mysql"
  "/usr/lib64/mysql"
  "/usr/local/lib/mysql"
  "/usr/local/lib64/mysql")

# Look for the program.
find_program(MYSQL_EXECUTABLE NAMES mysql_config
  HINTS "$ENV{MYSQL_HOME}/bin")

# Handle the QUIETLY and REQUIRED arguments and set MYSQL_FOUND to TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MySQL
  FOUND_VAR     MYSQL_FOUND
  REQUIRED_VARS MYSQL_INCLUDE_DIR
  MYSQL_LIBRARY
  MYSQL_EXECUTABLE
  FAIL_MESSAGE  "Failed to find MySQL")

mark_as_advanced(MYSQL_INCLUDE_DIR MYSQL_LIBRARY MYSQL_EXECUTABLE)

# Copy the results to the output variables.
if(MYSQL_FOUND)
  set(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
  set(MYSQL_LIBRARIES ${MYSQL_LIBRARY})
endif()
