# The Restful Matching-Engine.
# Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

# - Find Swirly
# Find the native Swirly headers and libraries.
#
#  SWIRLY_INCLUDE_DIRS - Where to find swirly/Config.hpp, etc.
#  SWIRLY_LIBRARIES    - List of Swirly libraries.
#  SWIRLY_FOUND        - True if Swirly found.

# Look for the header file.
FIND_PATH(SWIRLY_INCLUDE_DIR NAMES swirly/Config.hpp
          HINTS "$ENV{SWIRLY_HOME}/include")

# Look for the libraries.
FIND_LIBRARY(SWIRLY_UNIT_LIBRARY NAMES swirly_unit
             HINTS "$ENV{SWIRLY_HOME}/lib")
FIND_LIBRARY(SWIRLY_WS_LIBRARY NAMES swirly_ws
             HINTS "$ENV{SWIRLY_HOME}/lib")
FIND_LIBRARY(SWIRLY_CLOB_LIBRARY NAMES swirly_clob
             HINTS "$ENV{SWIRLY_HOME}/lib")
FIND_LIBRARY(SWIRLY_FIN_LIBRARY NAMES swirly_fin
             HINTS "$ENV{SWIRLY_HOME}/lib")
FIND_LIBRARY(SWIRLY_UTIL_LIBRARY NAMES swirly_util
             HINTS "$ENV{SWIRLY_HOME}/lib")

# Handle the QUIETLY and REQUIRED arguments and set SWIRLY_FOUND to TRUE if all listed variables are TRUE. 
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Swirly DEFAULT_MSG
  SWIRLY_INCLUDE_DIR
  SWIRLY_WS_LIBRARY
  SWIRLY_CLOB_LIBRARY
  SWIRLY_FIN_LIBRARY
  SWIRLY_UTIL_LIBRARY
  SWIRLY_UNIT_LIBRARY)

MARK_AS_ADVANCED(SWIRLY_INCLUDE_DIR SWIRLY_UNIT_LIBRARY SWIRLY_WS_LIBRARY SWIRLY_CLOB_LIBRARY SWIRLY_FIN_LIBRARY SWIRLY_UTIL_LIBRARY)

# Copy the results to the output variables.
IF(SWIRLY_FOUND)
  SET(SWIRLY_INCLUDE_DIRS ${SWIRLY_INCLUDE_DIR})
  SET(SWIRLY_LIBRARIES ${SWIRLY_WS_LIBRARY} ${SWIRLY_CLOB_LIBRARY} ${SWIRLY_FIN_LIBRARY} ${SWIRLY_UTIL_LIBRARY})
ENDIF()
