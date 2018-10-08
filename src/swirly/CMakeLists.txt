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

configure_file(Config.h.in "${CMAKE_BINARY_DIR}/include/swirly/Config.h")
install(FILES "${CMAKE_BINARY_DIR}/include/swirly/Config.h"
  DESTINATION include/swirly
  COMPONENT header
)

add_subdirectory(contrib)
add_subdirectory(util)
add_subdirectory(hdr)
add_subdirectory(sys)
add_subdirectory(app)
add_subdirectory(http)
add_subdirectory(fin)
add_subdirectory(fix)
add_subdirectory(lob)
add_subdirectory(db)
