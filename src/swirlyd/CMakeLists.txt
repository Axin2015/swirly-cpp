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
  HttpConn.cpp
  HttpRequest.cpp
  HttpServ.cpp
  Page.cpp
  RestApp.cpp
  RestBody.cpp
  RestImpl.cpp)

add_library(swirlyd-static STATIC ${lib_SOURCES})
set_target_properties(swirlyd-static PROPERTIES OUTPUT_NAME swirlyd)
target_link_libraries(swirlyd-static swirly-lob-static swirly-http-static)
install(TARGETS swirlyd-static DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT static)

set(test_SOURCES
  EntitySet.ut.cpp
  Page.ut.cpp
  RestBody.ut.cpp)

add_executable(swirlyd-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirlyd-test swirlyd-static ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME swirlyd::${name}Suite COMMAND swirlyd-test -l error -t ${name}Suite)
endforeach()

set(prog_SOURCES
  Main.cpp)

add_executable(swirlyd ${prog_SOURCES})
target_link_libraries(swirlyd ${swirly_db_LIBRARY} swirlyd-static stdc++fs)
install(TARGETS swirlyd DESTINATION bin COMPONENT program)
