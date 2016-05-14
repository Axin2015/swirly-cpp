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

INCLUDE(AddFileDependencies)

SET(dbfiles)
IF(SQLITE3_EXECUTABLE)

  MESSAGE(STATUS "Sqlite3 found: ${SQLITE3_EXECUTABLE}")

  SET(names
    coal
    coffee
    corp
    forex
  )

  FOREACH(name ${names})

    SET(dbfile "${PROJECT_BINARY_DIR}/db/${name}.db")

    ADD_CUSTOM_COMMAND(
      OUTPUT "${dbfile}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND ${CMAKE_COMMAND} -E remove "${dbfile}"
      COMMAND "${SQLITE3_EXECUTABLE}" "${dbfile}" <schema.sql
      COMMAND "${SQLITE3_EXECUTABLE}" "${dbfile}" <${name}.sql
      DEPENDS schema.sql ${name}.sql)

    SET(dbfiles ${dbfiles} "${dbfile}")
  ENDFOREACH()

ENDIF()

ADD_CUSTOM_TARGET(db ALL DEPENDS ${dbfiles})

FILE(GLOB sqlfiles "*.sql")
INSTALL(FILES ${sqlfiles}  DESTINATION db)
INSTALL(FILES ${dbfiles} DESTINATION db)
