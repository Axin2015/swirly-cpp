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
IF(DOT_EXECUTABLE)

  MESSAGE(STATUS "Dot found: ${DOT_EXECUTABLE}")

  SET(names
    Ash
    Elm
    Fig
    Fir
  )

  FOREACH(name ${names})

    SET(outfile ${name}Depend.png)

    STRING(TOLOWER ${name} lname)
    ADD_CUSTOM_COMMAND(
      OUTPUT "${PROJECT_BINARY_DIR}/image/${outfile}"
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      COMMAND CXX="${CMAKE_CXX_COMPILER}" "${PERL}" etc/dotdepend.pl ${lname}
              | dot -Tpng >"${PROJECT_BINARY_DIR}/image/${outfile}"
      DEPENDS "${PROJECT_SOURCE_DIR}/etc/dotdepend.pl"
              ${lname}_static)

    SET(outfiles ${outfiles} "${PROJECT_BINARY_DIR}/image/${outfile}")
  ENDFOREACH()

ENDIF()

ADD_CUSTOM_TARGET(image ALL DEPENDS ${outfiles})
