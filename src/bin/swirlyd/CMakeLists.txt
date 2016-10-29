# The Restful Matching-Engine.
# Copyright (C) 2013, 2016 Swirly Cloud Limited.
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

# Mongoose does not compile with strict aliasing.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")

if(MG_ENABLE_DEBUG)
  add_definitions(-DMG_ENABLE_DEBUG)
else()
  add_definitions(-DMG_DISABLE_STDIO)
endif()

add_definitions(-DMG_DISABLE_CGI)
add_definitions(-DMG_DISABLE_COAP)
add_definitions(-DMG_DISABLE_DAV)
add_definitions(-DMG_DISABLE_DAV_AUTH)
add_definitions(-DMG_DISABLE_DIRECTORY_LISTING)
add_definitions(-DMG_DISABLE_DNS)
add_definitions(-DMG_DISABLE_FILESYSTEM)
add_definitions(-DMG_DISABLE_HEXDUMP)
#add_definitions(-DMG_DISABLE_HTTP)
add_definitions(-DMG_DISABLE_HTTP_DIGEST_AUTH)
#add_definitions(-DMG_DISABLE_HTTP_KEEP_ALIVE)
add_definitions(-DMG_DISABLE_HTTP_WEBSOCKET)
add_definitions(-DMG_DISABLE_JSON_RPC)
add_definitions(-DMG_DISABLE_MQTT)
add_definitions(-DMG_DISABLE_PFS)
add_definitions(-DMG_DISABLE_POPEN)
add_definitions(-DMG_DISABLE_RESOLVE)
add_definitions(-DMG_DISABLE_RESOLVER)
#add_definitions(-DMG_DISABLE_SOCKETPAIR)
#add_definitions(-DMG_DISABLE_SOCKET_IF)
add_definitions(-DMG_DISABLE_SSI)
add_definitions(-DMG_DISABLE_STDIO)
add_definitions(-DMG_DISABLE_SYNC_RESOLVER)
add_definitions(-DMG_DISABLE_WS_RANDOM_MASK)

file(GLOB swirlyd_SOURCES "*.cpp")

add_executable(swirlyd ${swirlyd_SOURCES} "${PROJECT_SOURCE_DIR}/contrib/mongoose.c")
target_link_libraries(swirlyd ${ws_LIBRARY} ${sqlite_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
install(TARGETS swirlyd DESTINATION bin)
