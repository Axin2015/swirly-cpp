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

add_custom_target(swirly-bench DEPENDS
  swirly-fix-bench
  swirly-lob-bench
  swirly-micro-bench
  swirly-queue-bench
  swirly-timer-bench
)

add_executable(swirly-fix-bench FixBench.cpp)
target_link_libraries(swirly-fix-bench ${swirly_hdr_LIBRARY} ${swirly_fix_LIBRARY})

add_executable(swirly-lob-bench LobBench.cpp)
target_link_libraries(swirly-lob-bench ${swirly_hdr_LIBRARY} ${swirly_lob_LIBRARY}
  ${swirly_db_LIBRARY})

add_executable(swirly-micro-bench MicroBench.cpp)
target_link_libraries(swirly-micro-bench ${swirly_hdr_LIBRARY} ${swirly_sys_LIBRARY})

add_executable(swirly-queue-bench QueueBench.cpp)
target_link_libraries(swirly-queue-bench ${swirly_hdr_LIBRARY} ${swirly_app_LIBRARY})

add_executable(swirly-timer-bench TimerBench.cpp)
target_link_libraries(swirly-timer-bench ${swirly_hdr_LIBRARY} ${swirly_sys_LIBRARY})

add_custom_target(fix-benchmark DEPENDS
swirly-fix-bench
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
COMMAND bash -c "${CMAKE_INSTALL_BINDIR}/swirly-fix-bench \
  && \"${PROJECT_SOURCE_DIR}/script/hist-plot.sh\" \
  fix.hdr> fix-benchmark.png"
VERBATIM
)

add_custom_target(lob-benchmark DEPENDS
swirly-lob-bench
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
COMMAND bash -c "${CMAKE_INSTALL_BINDIR}/swirly-lob-bench \
  && \"${PROJECT_SOURCE_DIR}/script/hist-plot.sh\" \
  lob-maker.hdr lob-taker.hdr> lob-benchmark.png"
VERBATIM
)

add_custom_target(queue-benchmark DEPENDS
swirly-queue-bench
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
COMMAND bash -c "${CMAKE_INSTALL_BINDIR}/swirly-queue-bench \
  && \"${PROJECT_SOURCE_DIR}/script/hist-plot.sh\" \
  queue.hdr> queue-benchmark.png"
VERBATIM
)

add_custom_target(timer-benchmark DEPENDS
swirly-timer-bench
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
COMMAND bash -c "${CMAKE_INSTALL_BINDIR}/swirly-timer-bench \
  && \"${PROJECT_SOURCE_DIR}/script/hist-plot.sh\" \
  timer-replace.hdr timer-reset.hdr> timer-benchmark.png"
VERBATIM
)

add_custom_target(benchmark DEPENDS
fix-benchmark
lob-benchmark
queue-benchmark
timer-benchmark
)
