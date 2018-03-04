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
  add_custom_target(swirly_util DEPENDS
    swirly_util_static
    swirly_util_shared
  )
else()
  add_custom_target(swirly_util DEPENDS
    swirly_util_static
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

add_library(swirly_util_static STATIC ${lib_SOURCES})
set_target_properties(swirly_util_static PROPERTIES OUTPUT_NAME swirly_util)
target_link_libraries(swirly_util_static swirly_sys_static pthread)
install(TARGETS swirly_util_static DESTINATION lib COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly_util_shared SHARED ${lib_SOURCES})
  set_target_properties(swirly_util_shared PROPERTIES OUTPUT_NAME swirly_util)
  target_link_libraries(swirly_util_shared swirly_sys_shared pthread)
  install(TARGETS swirly_util_shared DESTINATION lib COMPONENT shared)
endif()

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

foreach(file ${test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  set(name "swirly_util_${name}")
  add_executable(${name} ${file})
  target_link_libraries(${name} ${swirly_util_LIBRARY} ${swirly_unit_LIBRARY})
  add_test(NAME ${name} COMMAND ${name})
endforeach()
