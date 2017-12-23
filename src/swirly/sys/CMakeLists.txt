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

if(ENABLE_SHARED_LIBS)
  add_custom_target(swirly_sys DEPENDS
    swirly_sys_static
    swirly_sys_shared
  )
else()
  add_custom_target(swirly_sys DEPENDS
    swirly_sys_static
  )
endif()

set(lib_SOURCES
  Actor.cpp
  Buffer.cpp
  Daemon.cpp
  Error.cpp
  File.cpp
  Handle.cpp
  IoSocket.cpp
  IpAddress.cpp
  LocalAddress.cpp
  Log.cpp
  MemAlloc.cpp
  MemCtx.cpp
  MemPool.cpp
  MMap.cpp
  Msg.cpp
  Muxer.cpp
  Poll.cpp
  Reactor.cpp
  RefCount.cpp
  Socket.cpp
  System.cpp
  TcpAcceptor.cpp
  TcpSocket.cpp
  Time.cpp
  Timer.cpp
  Types.cpp
  UdpSocket.cpp)

add_library(swirly_sys_static STATIC ${lib_SOURCES})
set_target_properties(swirly_sys_static PROPERTIES OUTPUT_NAME swirly_sys)
target_link_libraries(swirly_sys_static ${Boost_SYSTEM_LIBRARY} pthread)
install(TARGETS swirly_sys_static DESTINATION lib COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly_sys_shared SHARED ${lib_SOURCES})
  set_target_properties(swirly_sys_shared PROPERTIES OUTPUT_NAME swirly_sys)
  target_link_libraries(swirly_sys_shared ${Boost_SYSTEM_LIBRARY} pthread)
  install(TARGETS swirly_sys_shared DESTINATION lib COMPONENT shared)
endif()

set(test_SOURCES
  BufferTest.cxx
  HandleTest.cxx
  IpAddressTest.cxx
  MemCtxTest.cxx
  ReactorTest.cxx
  RefCountTest.cxx
  SocketTest.cxx)

foreach(file ${test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  set(name "swirly_sys_${name}")
  add_executable(${name} ${file})
  target_link_libraries(${name} ${swirly_sys_LIBRARY} ${swirly_unit_LIBRARY})
  add_test(NAME ${name} COMMAND ${name})
endforeach()
