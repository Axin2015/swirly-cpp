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
  add_custom_target(swirly-fix DEPENDS
    swirly-fix-static
    swirly-fix-shared
    swirly-fix-test
  )
else()
  add_custom_target(swirly-fix DEPENDS
    swirly-fix-static
    swirly-fix-test
  )
endif()

set(lib_SOURCES
  Acceptor.cpp
  App.cpp
  Config.cpp
  Conn.cpp
  Ctx.cpp
  Field.cpp
  Handler.cpp
  Header.cpp
  Initiator.cpp
  Lexer.cpp
  Msg.cpp
  Parser.cpp
  Random.cpp
  SessId.cpp
  Stream.cpp
  Types.cpp)

add_library(swirly-fix-static STATIC ${lib_SOURCES})
set_target_properties(swirly-fix-static PROPERTIES OUTPUT_NAME swirly-fix)
target_link_libraries(swirly-fix-static swirly-app-static)
install(TARGETS swirly-fix-static DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-fix-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-fix-shared PROPERTIES OUTPUT_NAME swirly-fix)
  target_link_libraries(swirly-fix-shared swirly-app-shared)
  install(TARGETS swirly-fix-shared DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/fix
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  Header.ut.cpp
  Lexer.ut.cpp
  SessId.ut.cpp
  Stream.ut.cpp)

add_executable(swirly-fix-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-fix-test ${swirly_fix_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME fix::${name}Suite COMMAND swirly-fix-test -l error -t ${name}Suite)
endforeach()
