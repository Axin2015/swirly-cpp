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

FILE(GLOB tea_SOURCES "*.cpp")

ADD_LIBRARY(tea_static STATIC ${tea_SOURCES})
SET_TARGET_PROPERTIES(tea_static PROPERTIES OUTPUT_NAME swirly_tea)
TARGET_LINK_LIBRARIES(tea_static)
INSTALL(TARGETS tea_static DESTINATION lib)

IF(ENABLE_SHARED_LIBS)
  ADD_LIBRARY(tea_shared SHARED ${tea_SOURCES})
  SET_TARGET_PROPERTIES(tea_shared PROPERTIES OUTPUT_NAME swirly_tea)
  TARGET_LINK_LIBRARIES(tea_shared)
  INSTALL(TARGETS tea_shared DESTINATION lib)
ENDIF()
