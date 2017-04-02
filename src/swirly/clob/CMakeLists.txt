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

set(clob_SOURCES
  Accnt.cpp
  AsyncJourn.cpp
  Match.cpp
  Response.cpp
  Serv.cpp
  Test.cpp)

add_library(clob_static STATIC ${clob_SOURCES})
set_target_properties(clob_static PROPERTIES OUTPUT_NAME swirly_clob)
target_link_libraries(clob_static fin_static)
install(TARGETS clob_static DESTINATION lib)

if(ENABLE_SHARED_LIBS)
  add_library(clob_shared SHARED ${clob_SOURCES})
  set_target_properties(clob_shared PROPERTIES OUTPUT_NAME swirly_clob)
  target_link_libraries(clob_shared fin_shared)
  install(TARGETS clob_shared DESTINATION lib)
endif()

set(clob_test_SOURCES
  AccntTest.cpp
  AsyncJournTest.cpp
  MainTest.cpp
  MatchTest.cpp
  ResponseTest.cpp
  ServTest.cpp)

add_executable(swirly_clob_test ${clob_test_SOURCES})
target_link_libraries(swirly_clob_test ${clob_LIBRARY} ${unit_LIBRARY})

add_test(NAME clob_test COMMAND swirly_clob_test)
