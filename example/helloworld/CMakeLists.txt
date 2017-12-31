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

cmake_minimum_required(VERSION 3.0)
project(helloworld)

if(DEFINED ENV{SWIRLY_HOME})
  message("SWIRLY_HOME is set to $ENV{SWIRLY_HOME}")
else()
  message(FATAL_ERROR "SWIRLY_HOME is not set")
endif()

set(COMMON_WARN "-Wall -Werror -Wextra -Wstrict-aliasing=2 -Wno-unused-parameter")
set(COMMON_FLAGS "-fstrict-aliasing -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "-std=c++14 ${COMMON_FLAGS} ${COMMON_WARN} -fno-rtti")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "$ENV{SWIRLY_HOME}/etc")

find_package(Swirly REQUIRED)

include_directories(${SWIRLY_INCLUDE_DIRS})

add_executable(helloworld Main.cpp)
target_link_libraries(helloworld ${SWIRLY_UTIL_LIBRARY})
