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
  )
else()
  add_custom_target(swirly_clob DEPENDS
    swirly_clob_static
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
  AsyncJournTest.cxx
  ResponseTest.cxx
  ServTest.cxx)

foreach(file ${test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  set(name "swirly_clob_${name}")
  add_executable(${name} ${file})
  target_link_libraries(${name} ${swirly_clob_LIBRARY} ${swirly_unit_LIBRARY})
  add_test(NAME ${name} COMMAND ${name})
endforeach()
