# The Restful Matching-Engine.
# Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

add_subdirectory(unit)
add_subdirectory(sys)
add_subdirectory(util)
add_subdirectory(fin)
add_subdirectory(clob)
add_subdirectory(web)

# FIXME: the following libraries should be modules.
add_subdirectory(sqlite)

configure_file(Config.h.in "${PROJECT_BINARY_DIR}/include/swirly/Config.h")
install(FILES "${PROJECT_BINARY_DIR}/include/swirly/Config.h"
  DESTINATION include/swirly
  COMPONENT header
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  DESTINATION include
  COMPONENT header
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*.hpp"
  PATTERN "Doxyfile.h" EXCLUDE
)
