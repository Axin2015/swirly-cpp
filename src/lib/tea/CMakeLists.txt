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

file(GLOB tea_SOURCES "*.cpp")

add_library(tea_static STATIC ${tea_SOURCES})
set_target_properties(tea_static PROPERTIES OUTPUT_NAME swirly_tea)
target_link_libraries(tea_static)
install(TARGETS tea_static DESTINATION lib)

if(ENABLE_SHARED_LIBS)
  add_library(tea_shared SHARED ${tea_SOURCES})
  set_target_properties(tea_shared PROPERTIES OUTPUT_NAME swirly_tea)
  target_link_libraries(tea_shared)
  install(TARGETS tea_shared DESTINATION lib)
endif()
