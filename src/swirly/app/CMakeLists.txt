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

if(SWIRLY_BUILD_SHARED)
  add_custom_target(swirly-app DEPENDS
    swirly-app-static
    swirly-app-shared
    swirly-app-test
  )
else()
  add_custom_target(swirly-app DEPENDS
    swirly-app-static
    swirly-app-test
  )
endif()

set(lib_SOURCES
  Backoff.cpp
  MemAlloc.cpp
  MemCtx.cpp
  MemPool.cpp
  MemQueue.cpp
  Thread.cpp)

add_library(swirly-app-static STATIC ${lib_SOURCES})
set_target_properties(swirly-app-static PROPERTIES OUTPUT_NAME swirly-app)
target_link_libraries(swirly-app-static swirly-sys-static)
install(TARGETS swirly-app-static DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-app-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-app-shared PROPERTIES OUTPUT_NAME swirly-app)
  target_link_libraries(swirly-app-shared swirly-sys-shared)
  install(TARGETS swirly-app-shared DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/app
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  MemAlloc.ut.cpp
  MemCtx.ut.cpp)

add_executable(swirly-app-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-app-test ${swirly_app_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME app::${name}Suite COMMAND swirly-app-test -l error -t ${name}Suite)
endforeach()
