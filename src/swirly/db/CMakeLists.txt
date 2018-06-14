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

if(SWIRLY_BUILD_SHARED)
  add_custom_target(swirly-db DEPENDS
    swirly-db-static
    swirly-db-shared
  )
else()
  add_custom_target(swirly-db DEPENDS
    swirly-db-static
  )
endif()

set(lib_SOURCES
  DbCtx.cpp
  Exception.cpp
  Sql.cpp
  SqliteJourn.cpp
  SqliteModel.cpp
  Sqlite.cpp
  Types.cpp)

add_library(dblibs INTERFACE)
if(MYSQL_FOUND)
  set(lib_SOURCES ${lib_SOURCES}
    MySqlJourn.cpp
    MySqlModel.cpp
    MySql.cpp)
  include_directories(${SQLITE3_INCLUDE_DIRS} ${MYSQL_INCLUDE_DIRS})
  target_link_libraries(dblibs INTERFACE ${SQLITE3_LIBRARIES} ${MYSQL_LIBRARIES})
else()
  include_directories(${SQLITE3_INCLUDE_DIRS})
  target_link_libraries(dblibs INTERFACE ${SQLITE3_LIBRARIES})
endif()

add_library(swirly-db-static STATIC ${lib_SOURCES})
set_target_properties(swirly-db-static PROPERTIES OUTPUT_NAME swirly-db)
target_link_libraries(swirly-db-static swirly-fin-static dblibs)
install(TARGETS swirly-db-static DESTINATION lib64 COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-db-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-db-shared PROPERTIES OUTPUT_NAME swirly-db)
  target_link_libraries(swirly-db-shared swirly-fin-shared dblibs)
  install(TARGETS swirly-db-shared DESTINATION lib64 COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/db
      COMPONENT header
    )
  endif()
endforeach()
