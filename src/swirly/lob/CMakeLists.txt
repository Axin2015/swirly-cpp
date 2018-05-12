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
  add_custom_target(swirly-lob DEPENDS
    swirly-lob-static
    swirly-lob-shared
    swirly-lob-test
  )
else()
  add_custom_target(swirly-lob DEPENDS
    swirly-lob-static
    swirly-lob-test
  )
endif()

set(lib_SOURCES
  Accnt.cpp
  Match.cpp
  Response.cpp
  Serv.cpp
  Test.cpp)

add_library(swirly-lob-static STATIC ${lib_SOURCES})
set_target_properties(swirly-lob-static PROPERTIES OUTPUT_NAME swirly-lob)
target_link_libraries(swirly-lob-static swirly-fin-static)
install(TARGETS swirly-lob-static DESTINATION lib64 COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly-lob-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-lob-shared PROPERTIES OUTPUT_NAME swirly-lob)
  target_link_libraries(swirly-lob-shared swirly-fin-shared)
  install(TARGETS swirly-lob-shared DESTINATION lib64 COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/lob
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  Response.ut.cpp
  Serv.ut.cpp)

add_executable(swirly-lob-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-lob-test ${swirly_lob_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME lob::${name}Suite COMMAND swirly-lob-test -l error -t ${name}Suite)
endforeach()
