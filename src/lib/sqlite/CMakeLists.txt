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

FILE(GLOB sqlite_SOURCES "*.cpp")

INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIRS})

ADD_LIBRARY(sqlite_static STATIC ${sqlite_SOURCES})
SET_TARGET_PROPERTIES(sqlite_static PROPERTIES OUTPUT_NAME swirly_sqlite)
TARGET_LINK_LIBRARIES(sqlite_static elm_static ${SQLITE3_LIBRARIES})
INSTALL(TARGETS sqlite_static DESTINATION lib)

IF(ENABLE_SHARED_LIBS)
  ADD_LIBRARY(sqlite_shared SHARED ${sqlite_SOURCES})
  SET_TARGET_PROPERTIES(sqlite_shared PROPERTIES OUTPUT_NAME swirly_sqlite)
  TARGET_LINK_LIBRARIES(sqlite_shared elm_shared ${SQLITE3_LIBRARIES})
  INSTALL(TARGETS sqlite_shared DESTINATION lib)
ENDIF()
