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

add_custom_target(swirly-tool DEPENDS
  swirly-db-to-dsv
  swirly-db-to-json
  swirly-echo-serv
  swirly-queue-bench
  swirly-scratch
  swirly-serv-bench
  swirly-timer-bench
)

install(PROGRAMS ${bin_FILES} DESTINATION bin COMPONENT program)

add_executable(swirly-db-to-dsv DbToDsv.cpp)
target_link_libraries(swirly-db-to-dsv ${swirly_sqlite_LIBRARY})
install(TARGETS swirly-db-to-dsv DESTINATION bin COMPONENT program)

add_executable(swirly-db-to-json DbToJson.cpp)
target_link_libraries(swirly-db-to-json ${swirly_sqlite_LIBRARY})
install(TARGETS swirly-db-to-json DESTINATION bin COMPONENT program)

add_executable(swirly-echo-serv EchoServ.cpp)
target_link_libraries(swirly-echo-serv ${swirly_fix_LIBRARY})
install(TARGETS swirly-echo-serv DESTINATION bin COMPONENT program)

add_executable(swirly-queue-bench QueueBench.cpp)
target_link_libraries(swirly-queue-bench ${swirly_util_LIBRARY})
install(TARGETS swirly-queue-bench DESTINATION bin COMPONENT program)

# Reserved as an ad-hoc scratch pad.
add_executable(swirly-scratch Scratch.cpp)
target_link_libraries(swirly-scratch ${swirly_fin_LIBRARY})

add_executable(swirly-serv-bench ServBench.cpp)
target_link_libraries(swirly-serv-bench ${swirly_lob_LIBRARY} ${swirly_sqlite_LIBRARY})
install(TARGETS swirly-serv-bench DESTINATION bin COMPONENT program)

add_executable(swirly-timer-bench TimerBench.cpp)
target_link_libraries(swirly-timer-bench ${swirly_sys_LIBRARY})
install(TARGETS swirly-timer-bench DESTINATION bin COMPONENT program)
