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
  Finally.cpp
  IntWrapper.cpp
  Limits.cpp
  Log.cpp
  Math.cpp
  Options.cpp
  RefCount.cpp
  RingBuffer.cpp
  Set.cpp
  Slot.cpp
  Stream.cpp
  String.cpp
  StringBuf.cpp
  Struct.cpp
  Symbol.cpp
  Time.cpp
  Tokeniser.cpp
  Traits.cpp
  TypeTraits.cpp
  Types.cpp
  Utility.cpp
  VarSub.cpp
  Version.cpp)

add_library(swirly-util-static STATIC ${lib_SOURCES})
set_target_properties(swirly-util-static PROPERTIES OUTPUT_NAME swirly-util)
target_link_libraries(swirly-util-static)
install(TARGETS swirly-util-static DESTINATION lib64 COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-util-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-util-shared PROPERTIES OUTPUT_NAME swirly-util)
  target_link_libraries(swirly-util-shared)
  install(TARGETS swirly-util-shared DESTINATION lib64 COMPONENT shared)
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
  Options.ut.cpp
  RefCount.ut.cpp
  RingBuffer.ut.cpp
  Set.ut.cpp
  Slot.ut.cpp
  Stream.ut.cpp
  String.ut.cpp
  StringBuf.ut.cpp
  Struct.ut.cpp
  Symbol.ut.cpp
  Tokeniser.ut.cpp
  Traits.ut.cpp
  Utility.ut.cpp
  VarSub.ut.cpp
  Version.ut.cpp)

add_executable(swirly-util-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-util-test ${swirly_util_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME util::${name}Suite COMMAND swirly-util-test -l error -t ${name}Suite)
endforeach()
