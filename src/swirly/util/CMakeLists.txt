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

set(util_SOURCES
  Array.cpp
  BasicTypes.cpp
  Compare.cpp
  Conf.cpp
  Date.cpp
  Defs.cpp
  Enum.cpp
  Exception.cpp
  File.cpp
  Finally.cpp
  IntWrapper.cpp
  Limits.cpp
  Log.cpp
  Math.cpp
  MemAlloc.cpp
  MemMap.cpp
  MemPool.cpp
  Numeric.cpp
  Pipe.cpp
  Profile.cpp
  RefCounted.cpp
  RingBuffer.cpp
  Set.cpp
  Stream.cpp
  String.cpp
  Symbol.cpp
  System.cpp
  Time.cpp
  Tokeniser.cpp
  Types.cpp
  Utility.cpp
  VarSub.cpp)

add_library(util_static STATIC ${util_SOURCES})
set_target_properties(util_static PROPERTIES OUTPUT_NAME swirly_util)
target_link_libraries(util_static pthread)
install(TARGETS util_static DESTINATION lib)

if(ENABLE_SHARED_LIBS)
  add_library(util_shared SHARED ${util_SOURCES})
  set_target_properties(util_shared PROPERTIES OUTPUT_NAME swirly_util)
  target_link_libraries(util_shared pthread)
  install(TARGETS util_shared DESTINATION lib)
endif()

set(util_test_SOURCES
  ArrayTest.cxx
  ConfTest.cxx
  DateTest.cxx
  EnumTest.cxx
  ExceptionTest.cxx
  FinallyTest.cxx
  IntWrapperTest.cxx
  LogTest.cxx
  MathTest.cxx
  MemPoolTest.cxx
  PipeTest.cxx
  RefCountedTest.cxx
  RingBufferTest.cxx
  SetTest.cxx
  StreamTest.cxx
  StringTest.cxx
  SymbolTest.cxx
  TokeniserTest.cxx
  UtilityTest.cxx
  VarSubTest.cxx)

foreach(file ${util_test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  set(name "util${name}")
  add_executable(${name} ${file})
  target_link_libraries(${name} ${util_LIBRARY} ${unit_LIBRARY})
  add_test(NAME ${name} COMMAND ${name})
endforeach()
