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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(swirlyc)

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/etc")
SET(CMAKE_VERBOSE_MAKEFILE 1)

# Configuration options:
SET(ENABLE_SHARED_LIBS ON  CACHE BOOL   "Enable shared libs.")
SET(MG_ENABLE_DEBUG    OFF CACHE BOOL   "Enable Mongoose debug.")
SET(SWIRLY_DEPTH       3   CACHE STRING "Depth of book.")

IF(NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE DEBUG)
  #SET(CMAKE_BUILD_TYPE RELEASE)
  SET(CMAKE_BUILD_TYPE RELWITHDEBINFO)
  #SET(CMAKE_BUILD_TYPE MINSIZEREL)
ENDIF()

# Enable for use with clang-tidy.
IF(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
  SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
ENDIF()

STRING(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

SET(COMMON_WARN "-Wall -Werror -Wextra -Wstrict-aliasing=2 -Wno-unused-parameter -Wno-unused-variable")
SET(COMMON_FLAGS "-fstrict-aliasing -fvisibility=hidden")

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_C_FLAGS "-std=c11 ${COMMON_FLAGS} ${COMMON_WARN}")
  SET(CMAKE_CXX_FLAGS "-std=c++14 ${COMMON_FLAGS} ${COMMON_WARN} -fno-enforce-eh-specs -fnothrow-opt -fno-rtti")
ELSEIF("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  SET(CMAKE_C_FLAGS "-std=c11 ${COMMON_FLAGS} ${COMMON_WARN}")
  SET(CMAKE_CXX_FLAGS "-std=c++14 ${COMMON_FLAGS} ${COMMON_WARN} -fno-rtti")
ENDIF()
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

ADD_DEFINITIONS(-DSWIRLY_DEPTH=${SWIRLY_DEPTH})

ADD_DEFINITIONS(-DBOOST_NO_AUTO_PTR=1 -DBOOST_NO_RTTI=1 -DBOOST_NO_TYPEID=1)
IF(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  ADD_DEFINITIONS(-DSWIRLY_ENABLE_DEBUG=1)
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

FILE(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
FILE(MAKE_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/share/doc/html")

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/etc")

FIND_PROGRAM(DOT_EXECUTABLE dot)
FIND_PROGRAM(ENV_EXECUTABLE env)
FIND_PROGRAM(RAGEL_EXECUTABLE ragel)

FIND_PACKAGE(Boost 1.55 REQUIRED COMPONENTS date_time filesystem system)
FIND_PACKAGE(Doxygen)                # Optional.
FIND_PACKAGE(Perl)                   # Optional.
FIND_PACKAGE(PythonInterp 2.7 EXACT) # Optional.
FIND_PACKAGE(SQLite3 REQUIRED)

IF(DOXYGEN_FOUND)

  CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/Doxyfile.in"
    "${PROJECT_BINARY_DIR}/Doxyfile")

  ADD_CUSTOM_TARGET(doc
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/share"
    COMMAND "${DOXYGEN_EXECUTABLE}" "${PROJECT_BINARY_DIR}/Doxyfile"
    SOURCES "${PROJECT_BINARY_DIR}/Doxyfile")

  ADD_CUSTOM_COMMAND(TARGET doc POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "${PROJECT_SOURCE_DIR}/CNAME"
            "${PROJECT_BINARY_DIR}/share/doc/html/")
  ADD_DEPENDENCIES(doc image)

  INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/share/doc/" DESTINATION share/doc)
ENDIF()

INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/contrib")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")

IF(ENABLE_SHARED_LIBS)
  SET(ash_LIBRARY ash_shared)
  SET(elm_LIBRARY elm_shared)
  SET(fig_LIBRARY fig_shared)
  SET(fir_LIBRARY fir_shared)
  SET(tea_LIBRARY tea_shared)
  SET(sqlite_LIBRARY sqlite_shared)
ELSE()
  SET(ash_LIBRARY ash_static)
  SET(elm_LIBRARY elm_static)
  SET(fig_LIBRARY fig_static)
  SET(fir_LIBRARY fir_static)
  SET(tea_LIBRARY tea_static)
  SET(sqlite_LIBRARY sqlite_static)
ENDIF()

ENABLE_TESTING()

ADD_SUBDIRECTORY(etc)
ADD_SUBDIRECTORY(image)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(src)

ADD_SUBDIRECTORY(test)
