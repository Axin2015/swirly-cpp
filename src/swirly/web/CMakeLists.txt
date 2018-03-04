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

if(ENABLE_SHARED_LIBS)
  add_custom_target(swirly_web DEPENDS
    swirly_web_static
    swirly_web_shared
  )
else()
  add_custom_target(swirly_web DEPENDS
    swirly_web_static
  )
endif()

if(RAGEL_EXECUTABLE)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/RestBody.cpp"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND "${RAGEL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/RestBody.rl"
            -o "${CMAKE_CURRENT_SOURCE_DIR}/RestBody.cpp"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/RestBody.rl")
endif(RAGEL_EXECUTABLE)

set(lib_SOURCES
  EntitySet.cpp
  Exception.cpp
  HttpParser.cpp
  HttpRequest.cpp
  HttpResponse.cpp
  Page.cpp
  RestBody.cpp
  Rest.cpp
  Url.cpp
  "${PROJECT_SOURCE_DIR}/src/swirly/contrib/http_parser.c")

add_library(swirly_web_static STATIC ${lib_SOURCES})
set_target_properties(swirly_web_static PROPERTIES OUTPUT_NAME swirly_web)
target_link_libraries(swirly_web_static swirly_clob_static)
install(TARGETS swirly_web_static DESTINATION lib COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly_web_shared SHARED ${lib_SOURCES})
  set_target_properties(swirly_web_shared PROPERTIES OUTPUT_NAME swirly_web)
  target_link_libraries(swirly_web_shared swirly_clob_shared)
  install(TARGETS swirly_web_shared DESTINATION lib COMPONENT shared)
endif()

set(test_SOURCES
  EntitySet.ut.cpp
  HttpParser.ut.cpp
  Page.ut.cpp
  RestBody.ut.cpp
  Url.ut.cpp)

foreach(file ${test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  set(name "swirly_web_${name}")
  add_executable(${name} ${file})
  target_link_libraries(${name} ${swirly_web_LIBRARY} ${swirly_unit_LIBRARY})
  add_test(NAME ${name} COMMAND ${name})
endforeach()
