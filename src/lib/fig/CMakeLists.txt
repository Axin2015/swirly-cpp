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

FILE(GLOB fig_SOURCES "*.cpp")

ADD_LIBRARY(fig_static STATIC ${fig_SOURCES})
SET_TARGET_PROPERTIES(fig_static PROPERTIES OUTPUT_NAME swirly_fig)
TARGET_LINK_LIBRARIES(fig_static elm_static)
INSTALL(TARGETS fig_static DESTINATION lib)

IF(ENABLE_SHARED_LIBS)
  ADD_LIBRARY(fig_shared SHARED ${fig_SOURCES})
  SET_TARGET_PROPERTIES(fig_shared PROPERTIES OUTPUT_NAME swirly_fig)
  TARGET_LINK_LIBRARIES(fig_shared elm_shared)
  INSTALL(TARGETS fig_shared DESTINATION lib)
ENDIF()
