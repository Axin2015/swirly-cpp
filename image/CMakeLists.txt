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

include(AddFileDependencies)

set(outfiles)
if(DOT_EXECUTABLE)

  message(STATUS "Dot found: ${DOT_EXECUTABLE}")

  set(names
    util
    sys
    app
    http
    fix
    fin
    lob
  )

  foreach(name ${names})

    set(outfile "${PROJECT_BINARY_DIR}/image/swirly-${name}.png")

    add_custom_command(
      OUTPUT "${outfile}"
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      COMMAND CXX="${CMAKE_CXX_COMPILER}" "${PERL}" script/dotdepend.pl ${name}
              | "${DOT_EXECUTABLE}" -Tpng >"${outfile}"
      DEPENDS "${PROJECT_SOURCE_DIR}/script/dotdepend.pl" "swirly-${name}-static")

    set(outfiles ${outfiles} "${outfile}")
    install(FILES "${outfile}" DESTINATION image COMPONENT doc)
  endforeach()

endif()

add_custom_target(image ALL DEPENDS ${outfiles})
