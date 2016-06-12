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

file(GLOB sqlite_SOURCES "*.cpp")

include_directories(${SQLITE3_INCLUDE_DIRS})

add_library(sqlite_static STATIC ${sqlite_SOURCES})
set_target_properties(sqlite_static PROPERTIES OUTPUT_NAME swirly_sqlite)
target_link_libraries(sqlite_static elm_static ${SQLITE3_LIBRARIES})
install(TARGETS sqlite_static DESTINATION lib)

if(ENABLE_SHARED_LIBS)
  add_library(sqlite_shared SHARED ${sqlite_SOURCES})
  set_target_properties(sqlite_shared PROPERTIES OUTPUT_NAME swirly_sqlite)
  target_link_libraries(sqlite_shared elm_shared ${SQLITE3_LIBRARIES})
  install(TARGETS sqlite_shared DESTINATION lib)
endif()
