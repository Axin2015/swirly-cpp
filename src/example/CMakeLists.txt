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

add_custom_target(swirly-example DEPENDS
  swirly-echo-clnt
  swirly-echo-serv
  swirly-fix-app
  swirly-fix-parse
  swirly-http-serv
  swirly-scratch
)

add_executable(swirly-echo-clnt EchoClnt.cpp)
target_link_libraries(swirly-echo-clnt ${swirly_app_LIBRARY})

add_executable(swirly-echo-serv EchoServ.cpp)
target_link_libraries(swirly-echo-serv ${swirly_app_LIBRARY})

add_executable(swirly-fix-app FixApp.cpp)
target_link_libraries(swirly-fix-app ${swirly_fix_LIBRARY})
install(TARGETS swirly-fix-app DESTINATION bin COMPONENT program)

add_executable(swirly-fix-parse FixParse.cpp)
target_link_libraries(swirly-fix-parse ${swirly_fix_LIBRARY})
install(TARGETS swirly-fix-parse DESTINATION bin COMPONENT program)

add_executable(swirly-http-serv HttpServ.cpp)
target_link_libraries(swirly-http-serv ${swirly_http_LIBRARY})

# Reserved as an ad-hoc scratch pad.
add_executable(swirly-scratch Scratch.cpp)
target_link_libraries(swirly-scratch ${swirly_db_LIBRARY})
