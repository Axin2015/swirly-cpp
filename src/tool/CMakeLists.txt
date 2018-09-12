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
  swirly-app-bench
  swirly-db-to-dsv
  swirly-db-to-json
  swirly-queue-bench
  swirly-timer-bench
)

add_executable(swirly-app-bench AppBench.cpp)
target_link_libraries(swirly-app-bench ${swirly_prof_LIBRARY} ${swirly_lob_LIBRARY}
  ${swirly_db_LIBRARY})
install(TARGETS swirly-app-bench DESTINATION bin COMPONENT program)

add_executable(swirly-db-to-dsv DbToDsv.cpp)
target_link_libraries(swirly-db-to-dsv ${swirly_db_LIBRARY})
install(TARGETS swirly-db-to-dsv DESTINATION bin COMPONENT program)

add_executable(swirly-db-to-json DbToJson.cpp)
target_link_libraries(swirly-db-to-json ${swirly_db_LIBRARY})
install(TARGETS swirly-db-to-json DESTINATION bin COMPONENT program)

add_executable(swirly-queue-bench QueueBench.cpp)
target_link_libraries(swirly-queue-bench ${swirly_prof_LIBRARY} ${swirly_app_LIBRARY})
install(TARGETS swirly-queue-bench DESTINATION bin COMPONENT program)

add_executable(swirly-timer-bench TimerBench.cpp)
target_link_libraries(swirly-timer-bench ${swirly_sys_LIBRARY})
install(TARGETS swirly-timer-bench DESTINATION bin COMPONENT program)

add_custom_target(queue-benchmark DEPENDS
swirly-queue-bench
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
COMMAND bash -c "${CMAKE_INSTALL_BINDIR}/swirly-queue-bench && \"${CMAKE_SOURCE_DIR}/script/hist-plot.sh\" queue.hdr > queue_benchmark.png"
VERBATIM
)

add_custom_target(app-benchmark DEPENDS
swirly-app-bench
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
COMMAND bash -c "${CMAKE_INSTALL_BINDIR}/swirly-app-bench && \"${CMAKE_SOURCE_DIR}/script/hist-plot.sh\" maker.hdr taker.hdr > app_benchmark.png"
VERBATIM
)

add_custom_target(benchmark DEPENDS
queue-benchmark
app-benchmark
)

