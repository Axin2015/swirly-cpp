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

cmake_minimum_required(VERSION 3.0)
project(swirly)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/etc")
set(CMAKE_VERBOSE_MAKEFILE 1)

# Configuration options:
set(ENABLE_SHARED_LIBS ON  CACHE BOOL   "Enable shared libs.")
set(MG_ENABLE_DEBUG    OFF CACHE BOOL   "Enable Mongoose debug.")
set(SWIRLY_DEPTH       3   CACHE STRING "Depth of book.")

if(NOT CMAKE_BUILD_TYPE)
  #set(CMAKE_BUILD_TYPE DEBUG)
  #set(CMAKE_BUILD_TYPE RELEASE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
  #set(CMAKE_BUILD_TYPE MINSIZEREL)
endif()

# Enable for use with clang-tidy.
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

set(COMMON_WARN "-Wall -Werror -Wextra -Wstrict-aliasing=2 -Wno-unused-parameter -Wno-unused-variable")
set(COMMON_FLAGS "-fstrict-aliasing -fvisibility=hidden")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 ${COMMON_FLAGS} ${COMMON_WARN}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ${COMMON_FLAGS} ${COMMON_WARN} -fno-enforce-eh-specs -fnothrow-opt -fno-rtti")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 ${COMMON_FLAGS} ${COMMON_WARN}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ${COMMON_FLAGS} ${COMMON_WARN} -fno-rtti")
endif()
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

add_definitions(-DSWIRLY_DEPTH=${SWIRLY_DEPTH})

add_definitions(-DBOOST_NO_AUTO_PTR=1 -DBOOST_NO_RTTI=1 -DBOOST_NO_TYPEID=1)
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  add_definitions(-DSWIRLY_ENABLE_DEBUG=1)
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
file(MAKE_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/share/doc/html")

find_program(DOT_EXECUTABLE dot)
find_program(ENV_EXECUTABLE env)
find_program(RAGEL_EXECUTABLE ragel)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost   1.55  REQUIRED COMPONENTS date_time filesystem system)
# The sqlite3_errstr() function was added in 3.7.15.
find_package(SQLite3 3.7.15 REQUIRED)

find_package(Doxygen)                # Optional.
find_package(Perl)                   # Optional.
find_package(PythonInterp 2.7 EXACT) # Optional.
# Find Qt components.
find_package(Qt5Widgets QUIET)
find_package(Qt5Network QUIET)

if(DOXYGEN_FOUND)

  configure_file("${PROJECT_SOURCE_DIR}/Doxyfile.in"
    "${PROJECT_BINARY_DIR}/Doxyfile")

  add_custom_target(doc
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/share"
    COMMAND "${DOXYGEN_EXECUTABLE}" "${PROJECT_BINARY_DIR}/Doxyfile"
    SOURCES "${PROJECT_BINARY_DIR}/Doxyfile")

  add_custom_command(TARGET doc POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "${PROJECT_SOURCE_DIR}/CNAME"
            "${PROJECT_BINARY_DIR}/share/doc/html/")
  add_dependencies(doc image)

  install(DIRECTORY "${PROJECT_BINARY_DIR}/share/doc/" DESTINATION share/doc)
endif()

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("${PROJECT_SOURCE_DIR}/contrib")
include_directories("${PROJECT_SOURCE_DIR}/include")

if(ENABLE_SHARED_LIBS)
  set(ash_LIBRARY ash_shared)
  set(elm_LIBRARY elm_shared)
  set(fig_LIBRARY fig_shared)
  set(fir_LIBRARY fir_shared)
  set(tea_LIBRARY tea_shared)
  set(sqlite_LIBRARY sqlite_shared)
else()
  set(ash_LIBRARY ash_static)
  set(elm_LIBRARY elm_static)
  set(fig_LIBRARY fig_static)
  set(fir_LIBRARY fir_static)
  set(tea_LIBRARY tea_static)
  set(sqlite_LIBRARY sqlite_static)
endif()

enable_testing()

add_subdirectory(db)
add_subdirectory(etc)
add_subdirectory(image)
add_subdirectory(include)
add_subdirectory(src)

add_subdirectory(test)

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "Swirly")
set(CPACK_PACKAGE_VENDOR "Swirly Cloud Limited")
set(CPACK_PACKAGE_CONTACT "support@swirlycloud.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Swirly - The Restful Matching-Engine")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")

include(CPack)
