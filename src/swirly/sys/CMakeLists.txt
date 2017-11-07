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

set(sys_SOURCES
  Buffer.cpp
  Daemon.cpp
  Error.cpp
  File.cpp
  Handle.cpp
  IoSocket.cpp
  IpAddress.cpp
  Log.cpp
  MMap.cpp
  MemAlloc.cpp
  MemCtx.cpp
  MemPool.cpp
  Muxer.cpp
  Poll.cpp
  Reactor.cpp
  RefCount.cpp
  Socket.cpp
  System.cpp
  TcpSocket.cpp
  Time.cpp
  UdpSocket.cpp)

add_library(sys_static STATIC ${sys_SOURCES})
set_target_properties(sys_static PROPERTIES OUTPUT_NAME swirly_sys)
target_link_libraries(sys_static ${Boost_SYSTEM_LIBRARY} pthread)
install(TARGETS sys_static DESTINATION lib)

if(ENABLE_SHARED_LIBS)
  add_library(sys_shared SHARED ${sys_SOURCES})
  set_target_properties(sys_shared PROPERTIES OUTPUT_NAME swirly_sys)
  target_link_libraries(sys_shared ${Boost_SYSTEM_LIBRARY} pthread)
  install(TARGETS sys_shared DESTINATION lib)
endif()

set(sys_test_SOURCES
  IpAddressTest.cxx
  RefCountTest.cxx
  HandleTest.cxx
  MemCtxTest.cxx)

foreach(file ${sys_test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  set(name "sys${name}")
  add_executable(${name} ${file})
  target_link_libraries(${name} ${sys_LIBRARY} ${unit_LIBRARY})
  add_test(NAME ${name} COMMAND ${name})
endforeach()
