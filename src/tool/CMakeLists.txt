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

add_custom_target(swirly_tool DEPENDS
  swirly_dbtodsv
  swirly_dbtojson
  swirly_queue_bench
  swirly_scratch
  swirly_serv_bench
  swirly_timer_bench
)

install(PROGRAMS ${bin_FILES} DESTINATION bin COMPONENT program)

add_executable(swirly_dbtodsv DbToDsv.cpp)
target_link_libraries(swirly_dbtodsv ${swirly_sqlite_LIBRARY})
install(TARGETS swirly_dbtodsv DESTINATION bin COMPONENT program)

add_executable(swirly_dbtojson DbToJson.cpp)
target_link_libraries(swirly_dbtojson ${swirly_sqlite_LIBRARY})
install(TARGETS swirly_dbtojson DESTINATION bin COMPONENT program)

add_executable(swirly_queue_bench QueueBench.cpp)
target_link_libraries(swirly_queue_bench ${swirly_util_LIBRARY})
install(TARGETS swirly_queue_bench DESTINATION bin COMPONENT program)

# Reserved as an ad-hoc scratch pad.
add_executable(swirly_scratch Scratch.cpp)
target_link_libraries(swirly_scratch ${swirly_fin_LIBRARY})

add_executable(swirly_serv_bench ServBench.cpp)
target_link_libraries(swirly_serv_bench ${swirly_clob_LIBRARY} ${swirly_sqlite_LIBRARY})
install(TARGETS swirly_serv_bench DESTINATION bin COMPONENT program)

add_executable(swirly_timer_bench TimerBench.cpp)
target_link_libraries(swirly_timer_bench ${swirly_util_LIBRARY})
install(TARGETS swirly_timer_bench DESTINATION bin COMPONENT program)
