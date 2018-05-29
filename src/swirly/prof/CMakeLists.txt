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
  add_custom_target(swirly-prof DEPENDS
    swirly-prof-static
    swirly-prof-shared
    swirly-prof-test
  )
else()
  add_custom_target(swirly-prof DEPENDS
    swirly-prof-static
    swirly-prof-test
  )
endif()

set(lib_SOURCES
  File.cpp
  HdrHistogram.cpp
  HdrLogWriter.cpp
  HdrRecorder.cpp)

add_library(swirly-prof-static STATIC ${lib_SOURCES})
set_target_properties(swirly-prof-static PROPERTIES OUTPUT_NAME swirly-prof)
target_link_libraries(swirly-prof-static swirly-util-static hdr_histogram_static pthread)
install(TARGETS swirly-prof-static DESTINATION lib64 COMPONENT static)

if(SWIRLY_BUILD_SHARED)
  add_library(swirly-prof-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-prof-shared PROPERTIES OUTPUT_NAME swirly-prof)
  target_link_libraries(swirly-prof-shared swirly-util-shared hdr_histogram pthread)
  install(TARGETS swirly-prof-shared DESTINATION lib64 COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/prof
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  )

add_executable(swirly-prof-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-prof-test ${swirly_prof_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME prof::${name}Suite COMMAND swirly-prof-test -l error -t ${name}Suite)
endforeach()
