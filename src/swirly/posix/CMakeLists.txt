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

set(posix_SOURCES
  File.cpp
  Handle.cpp
  MMap.cpp
  System.cpp
  Util.cpp)

add_library(posix_static STATIC ${posix_SOURCES})
set_target_properties(posix_static PROPERTIES OUTPUT_NAME swirly_posix)
target_link_libraries(posix_static pthread)
install(TARGETS posix_static DESTINATION lib)

if(ENABLE_SHARED_LIBS)
  add_library(posix_shared SHARED ${posix_SOURCES})
  set_target_properties(posix_shared PROPERTIES OUTPUT_NAME swirly_posix)
  target_link_libraries(posix_shared pthread)
  install(TARGETS posix_shared DESTINATION lib)
endif()

set(posix_test_SOURCES
  HandleTest.cxx)

foreach(file ${posix_test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  set(name "posix${name}")
  add_executable(${name} ${file})
  target_link_libraries(${name} ${posix_LIBRARY} ${unit_LIBRARY})
  add_test(NAME ${name} COMMAND ${name})
endforeach()
