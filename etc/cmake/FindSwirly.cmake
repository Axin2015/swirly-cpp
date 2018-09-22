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

# - Find Swirly
# Find the native Swirly headers and libraries.
#
#  SWIRLY_INCLUDE_DIRS - Where to find swirly/Config.h, etc.
#  SWIRLY_LIBRARIES    - List of Swirly libraries.
#  SWIRLY_FOUND        - True if Swirly found.

include(GNUInstallDirs)

# Look for the header file.
find_path(SWIRLY_INCLUDE_DIR NAMES swirly/Config.h
          HINTS "$ENV{SWIRLY_HOME}/include")

# Look for the libraries.
find_library(SWIRLY_DB_LIBRARY NAMES swirly_db
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")
find_library(SWIRLY_LOB_LIBRARY NAMES swirly_lob
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")
find_library(SWIRLY_FIN_LIBRARY NAMES swirly_fin
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")
find_library(SWIRLY_FIX_LIBRARY NAMES swirly_fix
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")
find_library(SWIRLY_HTTP_LIBRARY NAMES swirly_http
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")
find_library(SWIRLY_APP_LIBRARY NAMES swirly_app
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")
find_library(SWIRLY_SYS_LIBRARY NAMES swirly_sys
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")
find_library(SWIRLY_HDR_LIBRARY NAMES swirly_hdr
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")
find_library(SWIRLY_UTIL_LIBRARY NAMES swirly_util
             HINTS "$ENV{SWIRLY_HOME}/${CMAKE_INSTALL_LIBDIR}")

# Handle the QUIETLY and REQUIRED arguments and set SWIRLY_FOUND to TRUE if all listed variables are TRUE. 
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Swirly DEFAULT_MSG
  SWIRLY_INCLUDE_DIR
  SWIRLY_DB_LIBRARY
  SWIRLY_LOB_LIBRARY
  SWIRLY_FIN_LIBRARY
  SWIRLY_FIX_LIBRARY
  SWIRLY_HTTP_LIBRARY
  SWIRLY_APP_LIBRARY
  SWIRLY_SYS_LIBRARY
  SWIRLY_HDR_LIBRARY
  SWIRLY_UTIL_LIBRARY)

mark_as_advanced(
  SWIRLY_INCLUDE_DIR
  SWIRLY_DB_LIBRARY
  SWIRLY_LOB_LIBRARY
  SWIRLY_FIN_LIBRARY
  SWIRLY_FIX_LIBRARY
  SWIRLY_HTTP_LIBRARY
  SWIRLY_APP_LIBRARY
  SWIRLY_SYS_LIBRARY
  SWIRLY_HDR_LIBRARY
  SWIRLY_UTIL_LIBRARY)

# Copy the results to the output variables.
if(SWIRLY_FOUND)
  set(SWIRLY_INCLUDE_DIRS ${SWIRLY_INCLUDE_DIR})
  set(SWIRLY_LIBRARIES
    ${SWIRLY_DB_LIBRARY}
    ${SWIRLY_LOB_LIBRARY}
    ${SWIRLY_FIN_LIBRARY}
    ${SWIRLY_FIX_LIBRARY}
    ${SWIRLY_HTTP_LIBRARY}
    ${SWIRLY_APP_LIBRARY}
    ${SWIRLY_SYS_LIBRARY}
    ${SWIRLY_HDR_LIBRARY}
    ${SWIRLY_UTIL_LIBRARY})
endif()
