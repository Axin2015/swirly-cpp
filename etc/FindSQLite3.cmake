# The Restful Matching-Engine.
# Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
FIND_PATH(SQLITE3_INCLUDE_DIR NAMES sqlite3.h)

# Look for the library.
FIND_LIBRARY(SQLITE3_LIBRARY NAMES sqlite3)

# Handle the QUIETLY and REQUIRED arguments and set SQLITE3_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SQLITE3 DEFAULT_MSG
  SQLITE3_INCLUDE_DIR
  SQLITE3_LIBRARY)

MARK_AS_ADVANCED(SQLITE3_INCLUDE_DIR SQLITE3_LIBRARY)

# Copy the results to the output variables.
IF(SQLITE3_FOUND)
  SET(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
  SET(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})
ENDIF(SQLITE3_FOUND)
