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
  add_custom_target(swirly-sqlite DEPENDS
    swirly-sqlite-static
    swirly-sqlite-shared
  )
else()
  add_custom_target(swirly-sqlite DEPENDS
    swirly-sqlite-static
  )
endif()

set(lib_SOURCES
  Exception.cpp
  Journ.cpp
  Model.cpp
  Types.cpp
  Utility.cpp)

include_directories(${SQLITE3_INCLUDE_DIRS})

add_library(swirly-sqlite-static STATIC ${lib_SOURCES})
set_target_properties(swirly-sqlite-static PROPERTIES OUTPUT_NAME swirly-sqlite)
target_link_libraries(swirly-sqlite-static swirly-fin-static ${SQLITE3_LIBRARIES})
install(TARGETS swirly-sqlite-static DESTINATION lib64 COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly-sqlite-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-sqlite-shared PROPERTIES OUTPUT_NAME swirly-sqlite)
  target_link_libraries(swirly-sqlite-shared swirly-fin-shared ${SQLITE3_LIBRARIES})
  install(TARGETS swirly-sqlite-shared DESTINATION lib64 COMPONENT shared)
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
