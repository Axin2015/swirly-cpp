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

set(prog_SOURCES
  ConsumerInfo.cpp
  Cursored.cpp
  Disruptor.cpp
  EventHandler.cpp
  EventProcessor.cpp
  Executor.cpp
  Main.cpp
  RingBuffer.cpp
  Runnable.cpp
  SequenceBarrier.cpp
  Sequence.cpp
  Sequenced.cpp
  Sequencer.cpp
  Types.cpp)

add_executable(swirly_disrupt ${prog_SOURCES})
target_link_libraries(swirly_disrupt ${swirly_util_LIBRARY})
install(TARGETS swirly_disrupt DESTINATION bin COMPONENT program)
