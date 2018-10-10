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
  add_custom_target(swirly-hdr DEPENDS
    swirly-hdr-static
    swirly-hdr-shared
    swirly-hdr-test
  )
else()
  add_custom_target(swirly-hdr DEPENDS
    swirly-hdr-static
    swirly-hdr-test
  )
endif()

set(lib_SOURCES
  File.cpp
  Histogram.cpp
  IntervalRecorder.cpp
  LogReader.cpp
  LogWriter.cpp
  Recorder.cpp)

add_library(swirly-hdr-static STATIC ${lib_SOURCES})
set_target_properties(swirly-hdr-static PROPERTIES OUTPUT_NAME swirly-hdr)
target_link_libraries(swirly-hdr-static swirly-util-static hdr_histogram_static pthread)
install(TARGETS swirly-hdr-static DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-hdr-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-hdr-shared PROPERTIES OUTPUT_NAME swirly-hdr)
  target_link_libraries(swirly-hdr-shared swirly-util-shared hdr_histogram pthread)
  install(TARGETS swirly-hdr-shared DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/hdr
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  )

add_executable(swirly-hdr-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-hdr-test ${swirly_hdr_LIBRARY} "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME hdr::${name}Suite COMMAND swirly-hdr-test -l error -t ${name}Suite)
endforeach()
