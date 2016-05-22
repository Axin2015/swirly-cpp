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

SET(bin_FILES
  swirly_shell
)

INSTALL(PROGRAMS ${bin_FILES} DESTINATION bin)

ADD_EXECUTABLE(swirly_bench Bench.cpp)
TARGET_LINK_LIBRARIES(swirly_bench ${fig_LIBRARY} ${sqlite_LIBRARY})
INSTALL(TARGETS swirly_bench DESTINATION bin)

ADD_EXECUTABLE(swirly_dump Dump.cpp)
TARGET_LINK_LIBRARIES(swirly_dump ${sqlite_LIBRARY})
INSTALL(TARGETS swirly_dump DESTINATION bin)
