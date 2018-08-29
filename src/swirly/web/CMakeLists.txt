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
  add_custom_target(swirly-web DEPENDS
    swirly-web-static
    swirly-web-shared
    swirly-web-test
  )
else()
  add_custom_target(swirly-web DEPENDS
    swirly-web-static
    swirly-web-test
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
  Page.cpp
  Parser.cpp
  Request.cpp
  RestApp.cpp
  RestBody.cpp
  Stream.cpp
  Types.cpp
  Url.cpp
  "${PROJECT_SOURCE_DIR}/src/swirly/contrib/http_parser.c")

add_library(swirly-web-static STATIC ${lib_SOURCES})
set_target_properties(swirly-web-static PROPERTIES OUTPUT_NAME swirly-web)
target_link_libraries(swirly-web-static swirly-lob-static)
install(TARGETS swirly-web-static DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-web-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-web-shared PROPERTIES OUTPUT_NAME swirly-web)
  target_link_libraries(swirly-web-shared swirly-lob-shared)
  install(TARGETS swirly-web-shared DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/web
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  EntitySet.ut.cpp
  Page.ut.cpp
  Parser.ut.cpp
  RestBody.ut.cpp
  Types.ut.cpp
  Url.ut.cpp)

add_executable(swirly-web-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-web-test ${swirly_web_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME web::${name}Suite COMMAND swirly-web-test -l error -t ${name}Suite)
endforeach()
