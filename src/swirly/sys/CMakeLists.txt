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
  add_custom_target(swirly_sys DEPENDS
    swirly_sys_static
    swirly_sys_shared
    swirly_sys_test
  )
else()
  add_custom_target(swirly_sys DEPENDS
    swirly_sys_static
    swirly_sys_test
  )
endif()

set(lib_SOURCES
  Buffer.cpp
  Daemon.cpp
  Error.cpp
  Event.cpp
  EventFile.cpp
  EventHandler.cpp
  File.cpp
  Handle.cpp
  IoSocket.cpp
  IpAddress.cpp
  LocalAddress.cpp
  Log.cpp
  MMap.cpp
  Math.cpp
  MemAlloc.cpp
  MemCtx.cpp
  MemPool.cpp
  MemQueue.cpp
  Memory.cpp
  Muxer.cpp
  PidFile.cpp
  Poll.cpp
  Reactor.cpp
  RefCount.cpp
  Signal.cpp
  Socket.cpp
  System.cpp
  TcpAcceptor.cpp
  TcpSocket.cpp
  Time.cpp
  Timer.cpp
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
  Buffer.ut.cpp
  Handle.ut.cpp
  IpAddress.ut.cpp
  Math.ut.cpp
  MemCtx.ut.cpp
  Reactor.ut.cpp
  RefCount.ut.cpp
  Socket.ut.cpp)

add_executable(swirly_sys_test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly_sys_test ${swirly_sys_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  add_test(NAME sys::${name}Suite COMMAND swirly_sys_test -l error -t ${name}Suite)
endforeach()
