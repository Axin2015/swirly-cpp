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

if(SWIRLY_BUILD_SHARED)
  add_custom_target(swirly-http DEPENDS
    swirly-http-static
    swirly-http-shared
    swirly-http-test
  )
else()
  add_custom_target(swirly-http DEPENDS
    swirly-http-static
    swirly-http-test
  )
endif()

set(lib_SOURCES
  App.cpp
  Conn.cpp
  Parser.cpp
  Request.cpp
  Serv.cpp
  Stream.cpp
  Types.cpp
  Url.cpp
  "${PROJECT_SOURCE_DIR}/src/swirly/contrib/http_parser.c")

add_library(swirly-http-static STATIC ${lib_SOURCES})
set_target_properties(swirly-http-static PROPERTIES OUTPUT_NAME swirly-http)
target_link_libraries(swirly-http-static swirly-app-static)
install(TARGETS swirly-http-static DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-http-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-http-shared PROPERTIES OUTPUT_NAME swirly-http)
  target_link_libraries(swirly-http-shared swirly-app-shared)
  install(TARGETS swirly-http-shared DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/http
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  Parser.ut.cpp
  Types.ut.cpp
  Url.ut.cpp)

add_executable(swirly-http-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-http-test ${swirly_http_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME http::${name}Suite COMMAND swirly-http-test -l error -t ${name}Suite)
endforeach()
