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

SET(outfiles)
IF(SQLITE3_EXECUTABLE)

  MESSAGE(STATUS "Sqlite3 found: ${SQLITE3_EXECUTABLE}")

  SET(names
    coal
    coffee
    corp
    forex
  )

  FOREACH(name ${names})

    SET(outfile "${PROJECT_BINARY_DIR}/data/${name}.db")

    ADD_CUSTOM_COMMAND(
      OUTPUT "${outfile}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND ${CMAKE_COMMAND} -E remove "${outfile}"
      COMMAND "${SQLITE3_EXECUTABLE}" "${outfile}" <schema.sql
      COMMAND "${SQLITE3_EXECUTABLE}" "${outfile}" <${name}.sql
      DEPENDS schema.sql ${name}.sql)

    SET(outfiles ${outfiles} "${outfile}")
  ENDFOREACH()

ENDIF()

ADD_CUSTOM_TARGET(data ALL DEPENDS ${outfiles})

FILE(GLOB infiles "*.sql")
INSTALL(FILES ${infiles}  DESTINATION data)
INSTALL(FILES ${outfiles} DESTINATION data)
