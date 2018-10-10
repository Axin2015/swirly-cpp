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

# Version 3.1 required for CMAKE_CXX_STANDARD.
cmake_minimum_required(VERSION 3.8)
project(swirly-cpp)
include(GNUInstallDirs)

# Configuration options:
set(SWIRLY_BUILD_ARCH "native" CACHE STRING "Target architecture.")
set(SWIRLY_BUILD_SHARED ON CACHE BOOL "Enable shared libs.")
set(SWIRLY_MAX_LEVELS 3 CACHE STRING "Maximum price levels.")
set(SWIRLY_VERSION "0.1.0" CACHE STRING "Release version.")
set(SWIRLY_TOOLSET "" CACHE PATH "Toolchain prefix.")

# Flatbuffer options.
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "Enable the build of tests and samples.")
set(FLATBUFFERS_FLATC_SCHEMA_EXTRA_ARGS --no-prefix --scoped-enums)

# Hdr histogram options.
set(HDR_HISTOGRAM_BUILD_PROGRAMS OFF CACHE BOOL "Build tests and examples.")

if (NOT "${SWIRLY_TOOLSET}" STREQUAL "")
  get_filename_component(SWIRLY_TOOLSET "${SWIRLY_TOOLSET}" REALPATH)
  set(CMAKE_PREFIX_PATH "${SWIRLY_TOOLSET}")
  message(STATUS "SWIRLY_TOOLSET: ${SWIRLY_TOOLSET}")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/etc/cmake")

# Optimise for target architecture.
set(CMAKE_C_FLAGS_RELEASE          "${CMAKE_C_FLAGS_RELEASE} -march=${SWIRLY_BUILD_ARCH}")
set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} -march=${SWIRLY_BUILD_ARCH}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO} -march=${SWIRLY_BUILD_ARCH}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -march=${SWIRLY_BUILD_ARCH}")
set(CMAKE_C_FLAGS_MINSIZEREL       "${CMAKE_C_FLAGS_MINSIZEREL} -march=${SWIRLY_BUILD_ARCH}")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} -march=${SWIRLY_BUILD_ARCH}")

set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# Intended for use with Linux Perf:
# $ perf record -g swirly_bench
# $ perf report -g "graph,0.5,caller"
set(CMAKE_C_FLAGS_PERF   "${CMAKE_C_FLAGS_RELEASE} -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_PERF "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer")

if(NOT CMAKE_BUILD_TYPE)
  #set(CMAKE_BUILD_TYPE DEBUG)
  #set(CMAKE_BUILD_TYPE RELEASE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
  #set(CMAKE_BUILD_TYPE MINSIZEREL)
  #set(CMAKE_BUILD_TYPE PERF)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build architecture: ${SWIRLY_BUILD_ARCH}")

# Enable for use with clang-tidy.
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

set(CMAKE_C_STANDARD 11)
# Emits -std=gnu++1z on OSX which is not working as expected.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COMMON_WARN "-Wall -Werror -Wextra -Wstrict-aliasing=2 -Wno-unused-parameter -Wno-unused-variable")
set(COMMON_C_FLAGS "-fstrict-aliasing -fvisibility=hidden")
set(COMMON_CXX_FLAGS "${COMMON_C_FLAGS} -faligned-new -fno-rtti")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(COMMON_WARN "${COMMON_WARN} -Wno-implicit-fallthrough")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_C_FLAGS} ${COMMON_WARN}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_CXX_FLAGS} ${COMMON_WARN} -fno-enforce-eh-specs -fnothrow-opt")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_C_FLAGS} ${COMMON_WARN}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_CXX_FLAGS} ${COMMON_WARN}")
endif()
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  add_definitions(-DSWIRLY_BUILD_DEBUG=1)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/share/doc/html")

find_program(DOT_EXECUTABLE dot)
find_program(ENV_EXECUTABLE env)
find_program(RAGEL_EXECUTABLE ragel)

function(install_libraries)
  foreach(lib ${ARGN})
    if(NOT "${lib}" MATCHES "\\.a$")
      get_filename_component(dir "${lib}" DIRECTORY)
      get_filename_component(name "${lib}" NAME_WE)
      # Glob library and symlinks.
      file(GLOB files "${dir}/${name}.*")
      foreach (file ${files})
        if(NOT "${file}" MATCHES "\\.a$")
          message(STATUS "Runtime dependency: ${file}")
          install(FILES "${file}" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shared)
        endif()
      endforeach()
    endif()
  endforeach()
endfunction()

install(DIRECTORY DESTINATION var/log)
install(DIRECTORY DESTINATION var/run)

set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.63 REQUIRED COMPONENTS date_time system unit_test_framework)
install_libraries("${Boost_DATE_TIME_LIBRARY}" "${Boost_SYSTEM_LIBRARY}")

add_definitions(-DBOOST_NO_AUTO_PTR=1 -DBOOST_NO_RTTI=1 -DBOOST_NO_TYPEID=1)
add_definitions(-DBOOST_ASIO_DISABLE_THREADS=1 -DBOOST_ASIO_HEADER_ONLY=1)
if(NOT "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}" MATCHES "\\.a$")
  add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

find_package(FlatBuffers REQUIRED)

# The sqlite3_errstr() function was added in 3.7.15.
find_package(SQLite3 3.7.15 REQUIRED)
install_libraries(${SQLITE3_LIBRARIES})

find_package(MySQL 5.5)
if(MYSQL_FOUND)
  set(SWIRLY_HAVE_MYSQL 1)
  install_libraries(${MYSQL_LIBRARIES})
else()
  set(SWIRLY_HAVE_MYSQL 0)
endif()

find_package(Doxygen)                # Optional.
find_package(Perl)                   # Optional.
find_package(PythonInterp 2.7 EXACT) # Optional.

# Find Qt components.
find_package(Qt5Widgets QUIET)
find_package(Qt5Network QUIET)

if(DOXYGEN_FOUND)
  configure_file("${PROJECT_SOURCE_DIR}/Doxyfile.in"
    "${CMAKE_BINARY_DIR}/Doxyfile")

  add_custom_target(doc
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/share"
    COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/Doxyfile"
    SOURCES "${CMAKE_BINARY_DIR}/Doxyfile")

  add_dependencies(doc image)
endif()

include_directories(SYSTEM
  "${Boost_INCLUDE_DIRS}"
  "${FLATBUFFERS_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/HdrHistogram_c/src")

include_directories(
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_BINARY_DIR}/include")

if(SWIRLY_BUILD_SHARED)
  set(swirly_util_LIBRARY swirly-util-shared)
  set(swirly_hdr_LIBRARY swirly-hdr-shared)
  set(swirly_sys_LIBRARY swirly-sys-shared)
  set(swirly_app_LIBRARY swirly-app-shared)
  set(swirly_http_LIBRARY swirly-http-shared)
  set(swirly_fin_LIBRARY swirly-fin-shared)
  set(swirly_fix_LIBRARY swirly-fix-shared)
  set(swirly_lob_LIBRARY swirly-lob-shared)
  set(swirly_db_LIBRARY swirly-db-shared)
else()
  set(swirly_util_LIBRARY swirly-util-static)
  set(swirly_hdr_LIBRARY swirly-hdr-static)
  set(swirly_sys_LIBRARY swirly-sys-static)
  set(swirly_app_LIBRARY swirly-app-static)
  set(swirly_http_LIBRARY swirly-http-static)
  set(swirly_fin_LIBRARY swirly-fin-static)
  set(swirly_fix_LIBRARY swirly-fix-static)
  set(swirly_lob_LIBRARY swirly-lob-static)
  set(swirly_db_LIBRARY swirly-db-static)
endif()

enable_testing()

# Add cppcheck target if tool is available
find_program(CPPCHECK cppcheck HINTS "${SWIRLY_TOOLSET}/bin")
if (CPPCHECK)
  message(STATUS "Found cppcheck")
  cmake_host_system_information(RESULT CORE_COUNT QUERY NUMBER_OF_LOGICAL_CORES)
  configure_file("${PROJECT_SOURCE_DIR}/cppcheck.sh.in" "${CMAKE_BINARY_DIR}/cppcheck.sh")
  add_custom_target(cppcheck COMMAND bash "${CMAKE_BINARY_DIR}/cppcheck.sh")
endif()

# Add clang-format target if tool is available
find_program(CLANG_FORMAT clang-format HINTS "${SWIRLY_TOOLSET}/bin")
if (CLANG_FORMAT)
  message(STATUS "Found clang-format")
  configure_file("${PROJECT_SOURCE_DIR}/clang-format.sh.in" "${CMAKE_BINARY_DIR}/clang-format.sh")
  add_custom_target(clang-format COMMAND bash "${CMAKE_BINARY_DIR}/clang-format.sh")
endif()

# Add clang-tidy target if tool is available
find_program(CLANG_TIDY run-clang-tidy.py HINTS "/usr/share/clang/" "${SWIRLY_TOOLSET}/share/clang")
if (CLANG_TIDY)
  message(STATUS "Found clang-tidy")
  cmake_host_system_information(RESULT CORE_COUNT QUERY NUMBER_OF_LOGICAL_CORES)
  add_custom_target(clang-tidy COMMAND "${CLANG_TIDY}"
        -quiet
        -j ${CORE_COUNT}
        -p "${CMAKE_BINARY_DIR}")
endif()

add_subdirectory(HdrHistogram_c)
add_subdirectory(etc)
add_subdirectory(image)
add_subdirectory(src)

set(CPACK_GENERATOR "TGZ;RPM")

set(CPACK_PACKAGE_CONTACT "support@swirlycloud.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Swirly - The Restful Matching-Engine")
set(CPACK_PACKAGE_HOMEPAGE_URL "http://www.swirlycloud.com")
set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Swirly Cloud Limited")
set(CPACK_PACKAGE_VERSION "${SWIRLY_VERSION}")
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_PROCESSOR})

set(CPACK_ALL_INSTALL_TYPES Standard Developer)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL doc config header program shared static)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

# Runtime.

set(CPACK_COMPONENT_CONFIG_DISPLAY_NAME "Configuration")
set(CPACK_COMPONENT_CONFIG_GROUP "Runtime")
set(CPACK_COMPONENT_CONFIG_INSTALL_TYPES Standard Developer)

set(CPACK_COMPONENT_PROGRAM_DISPLAY_NAME "Programs")
set(CPACK_COMPONENT_PROGRAM_GROUP "Runtime")
set(CPACK_COMPONENT_PROGRAM_INSTALL_TYPES Standard Developer)

set(CPACK_COMPONENT_SHARED_DISPLAY_NAME "Shared Libs")
set(CPACK_COMPONENT_SHARED_GROUP "Runtime")
set(CPACK_COMPONENT_SHARED_INSTALL_TYPES Standard Developer)

# Development.

set(CPACK_COMPONENT_DOC_DISPLAY_NAME "Documentation")
set(CPACK_COMPONENT_DOC_GROUP "Development")
set(CPACK_COMPONENT_DOC_INSTALL_TYPES Standard Developer)

set(CPACK_COMPONENT_HEADER_DISPLAY_NAME "C++ Headers")
set(CPACK_COMPONENT_HEADER_GROUP "Development")
set(CPACK_COMPONENT_HEADER_INSTALL_TYPES Developer)
set(CPACK_COMPONENT_HEADER_DEPENDS static)

set(CPACK_COMPONENT_STATIC_DISPLAY_NAME "Static Libs")
set(CPACK_COMPONENT_STATIC_GROUP "Development")
set(CPACK_COMPONENT_STATIC_INSTALL_TYPES Developer)
set(CPACK_COMPONENT_STATIC_DEPENDS header)

set(CPACK_PROJECT_CONFIG_FILE "${PROJECT_SOURCE_DIR}/CPackProjectConfig.cmake")
include(CPack)
