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

# - Find SQLite3
# Find the native SQLite3 headers and libraries.
#
#  SQLITE3_INCLUDE_DIRS - Where to find sqlite3.h, etc.
#  SQLITE3_LIBRARIES    - List of SQLite3 libraries.
#  SQLITE3_FOUND        - True if SQLite3 found.

# Look for the header file.
find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h
          HINTS "$ENV{SQLITE3_HOME}/include")

# Look for the library.
find_library(SQLITE3_LIBRARY NAMES sqlite3
             HINTS "$ENV{SQLITE3_HOME}/lib")

# Look for the program.
find_program(SQLITE3_EXECUTABLE NAMES sqlite3
             HINTS "$ENV{SQLITE3_HOME}/bin")

if(SQLITE3_INCLUDE_DIR)
  file(STRINGS "${SQLITE3_INCLUDE_DIR}/sqlite3.h" _sqlite3_version_raw
       REGEX "^#define[\t ]+SQLITE_VERSION[\t ]+\"[0-9]+(\\.[0-9]+)+\"")
  string(REGEX REPLACE "^#define[\t ]+SQLITE_VERSION[\t ]+\"([0-9]+(\\.[0-9]+)+)\"" "\\1"
         SQLITE3_VERSION_STRING "${_sqlite3_version_raw}")
  unset(_sqlite3_version_raw)
endif()

# Handle the QUIETLY and REQUIRED arguments and set SQLITE3_FOUND to TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3
                                  FOUND_VAR     SQLITE3_FOUND
                                  REQUIRED_VARS SQLITE3_INCLUDE_DIR
                                                SQLITE3_LIBRARY
                                                SQLITE3_EXECUTABLE
                                  VERSION_VAR   SQLITE3_VERSION_STRING
                                  FAIL_MESSAGE  "Failed to find Sqlite3")

mark_as_advanced(SQLITE3_INCLUDE_DIR SQLITE3_LIBRARY SQLITE3_EXECUTABLE)

# Copy the results to the output variables.
if(SQLITE3_FOUND)
  set(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
  set(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})
endif()
