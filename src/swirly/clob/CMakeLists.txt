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
  add_custom_target(swirly_clob DEPENDS
    swirly_clob_static
    swirly_clob_shared
    swirly_clob_test
  )
else()
  add_custom_target(swirly_clob DEPENDS
    swirly_clob_static
    swirly_clob_test
  )
endif()

set(lib_SOURCES
  Accnt.cpp
  AsyncJourn.cpp
  Match.cpp
  Response.cpp
  Serv.cpp
  Test.cpp)

add_library(swirly_clob_static STATIC ${lib_SOURCES})
set_target_properties(swirly_clob_static PROPERTIES OUTPUT_NAME swirly_clob)
target_link_libraries(swirly_clob_static swirly_fin_static)
install(TARGETS swirly_clob_static DESTINATION lib COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly_clob_shared SHARED ${lib_SOURCES})
  set_target_properties(swirly_clob_shared PROPERTIES OUTPUT_NAME swirly_clob)
  target_link_libraries(swirly_clob_shared swirly_fin_shared)
  install(TARGETS swirly_clob_shared DESTINATION lib COMPONENT shared)
endif()

set(test_SOURCES
  AsyncJourn.ut.cpp
  Response.ut.cpp
  Serv.ut.cpp)

add_executable(swirly_clob_test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly_clob_test ${swirly_clob_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  add_test(NAME clob::${name}Suite COMMAND swirly_clob_test -l error -t ${name}Suite)
endforeach()
