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

if(ENABLE_SHARED_LIBS)
  add_custom_target(swirly_sqlite DEPENDS
    swirly_sqlite_static
    swirly_sqlite_shared
  )
else()
  add_custom_target(swirly_sqlite DEPENDS
    swirly_sqlite_static
  )
endif()

set(lib_SOURCES
  Exception.cpp
  Journ.cpp
  Model.cpp
  Types.cpp
  Utility.cpp)

include_directories(${SQLITE3_INCLUDE_DIRS})

add_library(swirly_sqlite_static STATIC ${lib_SOURCES})
set_target_properties(swirly_sqlite_static PROPERTIES OUTPUT_NAME swirly_sqlite)
target_link_libraries(swirly_sqlite_static swirly_fin_static ${SQLITE3_LIBRARIES})
install(TARGETS swirly_sqlite_static DESTINATION lib COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly_sqlite_shared SHARED ${lib_SOURCES})
  set_target_properties(swirly_sqlite_shared PROPERTIES OUTPUT_NAME swirly_sqlite)
  target_link_libraries(swirly_sqlite_shared swirly_fin_shared ${SQLITE3_LIBRARIES})
  install(TARGETS swirly_sqlite_shared DESTINATION lib COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/sqlite
      COMPONENT header
    )
  endif()
endforeach()
