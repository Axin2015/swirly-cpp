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

if(RAGEL_EXECUTABLE)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.cpp"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND "${RAGEL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.rl"
            -o "${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.cpp"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.rl")
endif(RAGEL_EXECUTABLE)

set(fir_SOURCES EntitySet.cpp Rest.cpp RestRequest.cpp)

add_library(fir_static STATIC ${fir_SOURCES})
set_target_properties(fir_static PROPERTIES OUTPUT_NAME swirly_fir)
target_link_libraries(fir_static fig_static)
install(TARGETS fir_static DESTINATION lib)

if(ENABLE_SHARED_LIBS)
  add_library(fir_shared SHARED ${fir_SOURCES})
  set_target_properties(fir_shared PROPERTIES OUTPUT_NAME swirly_fir)
  target_link_libraries(fir_shared fig_shared)
  install(TARGETS fir_shared DESTINATION lib)
endif()
