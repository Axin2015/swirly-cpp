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
  swirly-hdr-decoder
  swirly-hiccup
  swirly-lob-bench
  swirly-queue-bench
  swirly-timer-bench
)

add_executable(swirly-db-to-dsv DbToDsv.cpp)
target_link_libraries(swirly-db-to-dsv ${swirly_db_LIBRARY})
install(TARGETS swirly-db-to-dsv DESTINATION bin COMPONENT program)

add_executable(swirly-db-to-json DbToJson.cpp)
target_link_libraries(swirly-db-to-json ${swirly_db_LIBRARY})
install(TARGETS swirly-db-to-json DESTINATION bin COMPONENT program)

add_executable(swirly-hdr-decoder HdrDecoder.cpp)
target_link_libraries(swirly-hdr-decoder ${swirly_hdr_LIBRARY} ${swirly_sys_LIBRARY})
install(TARGETS swirly-hdr-decoder DESTINATION bin COMPONENT program)

add_executable(swirly-hiccup Hiccup.cpp)
target_link_libraries(swirly-hiccup ${swirly_hdr_LIBRARY} ${swirly_sys_LIBRARY})
install(TARGETS swirly-hiccup DESTINATION bin COMPONENT program)

add_executable(swirly-lob-bench LobBench.cpp)
target_link_libraries(swirly-lob-bench ${swirly_hdr_LIBRARY} ${swirly_lob_LIBRARY}
  ${swirly_db_LIBRARY})
install(TARGETS swirly-lob-bench DESTINATION bin COMPONENT program)

add_executable(swirly-queue-bench QueueBench.cpp)
target_link_libraries(swirly-queue-bench ${swirly_hdr_LIBRARY} ${swirly_app_LIBRARY})
install(TARGETS swirly-queue-bench DESTINATION bin COMPONENT program)

add_executable(swirly-timer-bench TimerBench.cpp)
target_link_libraries(swirly-timer-bench ${swirly_hdr_LIBRARY} ${swirly_sys_LIBRARY})
install(TARGETS swirly-timer-bench DESTINATION bin COMPONENT program)

add_custom_target(queue-benchmark DEPENDS
swirly-queue-bench
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
COMMAND bash -c "${CMAKE_INSTALL_BINDIR}/swirly-queue-bench && \"${CMAKE_SOURCE_DIR}/script/hist-plot.sh\" queue.hdr > queue_benchmark.png"
VERBATIM
)

add_custom_target(lob-benchmark DEPENDS
swirly-lob-bench
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
COMMAND bash -c "${CMAKE_INSTALL_BINDIR}/swirly-lob-bench && \"${CMAKE_SOURCE_DIR}/script/hist-plot.sh\" maker.hdr taker.hdr > lob_benchmark.png"
VERBATIM
)

add_custom_target(benchmark DEPENDS
queue-benchmark
lob-benchmark
)
