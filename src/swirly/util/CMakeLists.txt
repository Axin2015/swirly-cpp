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
  add_custom_target(swirly-util DEPENDS
    swirly-util-static
    swirly-util-shared
    swirly-util-test
  )
else()
  add_custom_target(swirly-util DEPENDS
    swirly-util-static
    swirly-util-test
  )
endif()

set(lib_SOURCES
  Array.cpp
  BasicTypes.cpp
  Compare.cpp
  Config.cpp
  Date.cpp
  Enum.cpp
  Exception.cpp
  File.cpp
  Finally.cpp
  IntWrapper.cpp
  Limits.cpp
  Log.cpp
  Math.cpp
  Pipe.cpp
  Profile.cpp
  RingBuffer.cpp
  Set.cpp
  Stream.cpp
  String.cpp
  StringBuf.cpp
  Symbol.cpp
  Tokeniser.cpp
  TypeTraits.cpp
  Types.cpp
  Utility.cpp
  VarSub.cpp)

add_library(swirly-util-static STATIC ${lib_SOURCES})
set_target_properties(swirly-util-static PROPERTIES OUTPUT_NAME swirly-util)
target_link_libraries(swirly-util-static swirly-sys-static pthread)
install(TARGETS swirly-util-static DESTINATION lib COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly-util-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-util-shared PROPERTIES OUTPUT_NAME swirly-util)
  target_link_libraries(swirly-util-shared swirly-sys-shared pthread)
  install(TARGETS swirly-util-shared DESTINATION lib COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/util
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  Array.ut.cpp
  Config.ut.cpp
  Date.ut.cpp
  Enum.ut.cpp
  Exception.ut.cpp
  Finally.ut.cpp
  IntWrapper.ut.cpp
  Log.ut.cpp
  Math.ut.cpp
  Pipe.ut.cpp
  RingBuffer.ut.cpp
  Set.ut.cpp
  Stream.ut.cpp
  String.ut.cpp
  StringBuf.ut.cpp
  Symbol.ut.cpp
  Tokeniser.ut.cpp
  Utility.ut.cpp
  VarSub.ut.cpp)

add_executable(swirly-util-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-util-test ${swirly_util_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME util::${name}Suite COMMAND swirly-util-test -l error -t ${name}Suite)
endforeach()
