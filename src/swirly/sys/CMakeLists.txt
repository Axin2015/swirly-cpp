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
  add_custom_target(swirly-sys DEPENDS
    swirly-sys-static
    swirly-sys-shared
    swirly-sys-test
  )
else()
  add_custom_target(swirly-sys DEPENDS
    swirly-sys-static
    swirly-sys-test
  )
endif()

set(lib_SOURCES
  Buffer.cpp
  Daemon.cpp
  Epoll.cpp
  EpollReactor.cpp
  Error.cpp
  Event.cpp
  File.cpp
  Handle.cpp
  IoSocket.cpp
  IpAddress.cpp
  LocalAddress.cpp
  MMap.cpp
  Memory.cpp
  Muxer.cpp
  PidFile.cpp
  Reactor.cpp
  Signal.cpp
  Socket.cpp
  System.cpp
  TcpAcceptor.cpp
  TcpConnector.cpp
  TcpSocket.cpp
  Timer.cpp
  UdpSocket.cpp)

add_library(swirly-sys-static STATIC ${lib_SOURCES})
set_target_properties(swirly-sys-static PROPERTIES OUTPUT_NAME swirly-sys)
target_link_libraries(swirly-sys-static swirly-util-static pthread)
install(TARGETS swirly-sys-static DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-sys-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-sys-shared PROPERTIES OUTPUT_NAME swirly-sys)
  target_link_libraries(swirly-sys-shared swirly-util-shared pthread)
  install(TARGETS swirly-sys-shared DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/sys
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  Buffer.ut.cpp
  EpollReactor.ut.cpp
  Handle.ut.cpp
  IpAddress.ut.cpp
  Socket.ut.cpp
  Timer.ut.cpp)

add_executable(swirly-sys-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-sys-test ${swirly_sys_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME sys::${name}Suite COMMAND swirly-sys-test -l error -t ${name}Suite)
endforeach()
