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

IF(RAGEL_EXECUTABLE)
  ADD_CUSTOM_COMMAND(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.cpp"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND "${RAGEL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.rl"
            -o "${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.cpp"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.rl")
ENDIF(RAGEL_EXECUTABLE)

FILE(GLOB fir_SOURCES "*.cpp")

ADD_LIBRARY(fir_static STATIC ${fir_SOURCES})
SET_TARGET_PROPERTIES(fir_static PROPERTIES OUTPUT_NAME swirly_fir)
TARGET_LINK_LIBRARIES(fir_static fig_static)
INSTALL(TARGETS fir_static DESTINATION lib)

IF(ENABLE_SHARED_LIBS)
  ADD_LIBRARY(fir_shared SHARED ${fir_SOURCES})
  SET_TARGET_PROPERTIES(fir_shared PROPERTIES OUTPUT_NAME swirly_fir)
  TARGET_LINK_LIBRARIES(fir_shared fig_shared)
  INSTALL(TARGETS fir_shared DESTINATION lib)
ENDIF()
