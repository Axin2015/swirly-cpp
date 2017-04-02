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
  Conf.cpp
  Exception.cpp
  Log.cpp
  MemAlloc.cpp
  MemPool.cpp
  Profile.cpp
  Stream.cpp
  String.cpp
  System.cpp
  Time.cpp
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
  ArrayTest.cpp
  BasicTypesTest.cpp
  CompareTest.cpp
  ConfTest.cpp
  DateTest.cpp
  DefsTest.cpp
  EnumTest.cpp
  ExceptionTest.cpp
  FinallyTest.cpp
  IntWrapperTest.cpp
  LimitsTest.cpp
  LogTest.cpp
  MainTest.cpp
  MathTest.cpp
  MemAllocTest.cpp
  MemPoolTest.cpp
  MnemTest.cpp
  NumericTest.cpp
  PipeTest.cpp
  ProfileTest.cpp
  RefCountedTest.cpp
  RingBufferTest.cpp
  SetTest.cpp
  StreamTest.cpp
  StringTest.cpp
  SystemTest.cpp
  TimeTest.cpp
  TokeniserTest.cpp
  TypesTest.cpp
  UtilityTest.cpp
  VarSubTest.cpp)

add_executable(swirly_util_test ${util_test_SOURCES})
target_link_libraries(swirly_util_test ${util_LIBRARY} ${unit_LIBRARY})

add_test(NAME util_test COMMAND swirly_util_test)
