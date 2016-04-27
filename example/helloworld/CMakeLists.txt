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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(helloworld)

IF(DEFINED ENV{SWIRLY_HOME})
  MESSAGE("SWIRLY_HOME is set to $ENV{SWIRLY_HOME}")
ELSE()
  MESSAGE(FATAL_ERROR "SWIRLY_HOME is not set")
ENDIF()

SET(COMMON_WARN "-Wall -Werror -Wextra -Wstrict-aliasing=2 -Wno-unused-parameter")
SET(COMMON_FLAGS "-fstrict-aliasing -fvisibility=hidden")
SET(CMAKE_CXX_FLAGS "-std=c++14 ${COMMON_FLAGS} ${COMMON_WARN} -fno-rtti")

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "$ENV{SWIRLY_HOME}/etc")

FIND_PACKAGE(Swirly REQUIRED)

INCLUDE_DIRECTORIES(${SWIRLY_INCLUDE_DIRS})

ADD_EXECUTABLE(helloworld Main.cpp)
TARGET_LINK_LIBRARIES(helloworld ${SWIRLY_ASH_LIBRARY})
