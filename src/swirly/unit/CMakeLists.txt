# The Restful Matching-Engine.
# Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

if(ENABLE_SHARED_LIBS)
  add_custom_target(swirly_unit DEPENDS
    swirly_unit_static
    swirly_unit_shared
  )
else()
  add_custom_target(swirly_unit DEPENDS
    swirly_unit_static
  )
endif()

set(lib_SOURCES
  Test.cpp)

add_library(swirly_unit_static STATIC ${lib_SOURCES})
set_target_properties(swirly_unit_static PROPERTIES OUTPUT_NAME swirly_unit)
target_link_libraries(swirly_unit_static)
install(TARGETS swirly_unit_static DESTINATION lib COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly_unit_shared SHARED ${lib_SOURCES})
  set_target_properties(swirly_unit_shared PROPERTIES OUTPUT_NAME swirly_unit)
  target_link_libraries(swirly_unit_shared)
  install(TARGETS swirly_unit_shared DESTINATION lib COMPONENT shared)
endif()
