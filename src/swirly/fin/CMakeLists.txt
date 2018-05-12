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

if(ENABLE_SHARED_LIBS)
  add_custom_target(swirly-fin DEPENDS
    swirly-fin-static
    swirly-fin-shared
    swirly-fin-test
  )
else()
  add_custom_target(swirly-fin DEPENDS
    swirly-fin-static
    swirly-fin-test
  )
endif()

set(lib_SOURCES
  Asset.cpp
  BasicTypes.cpp
  Conv.cpp
  Date.cpp
  DsvModel.cpp
  Exception.cpp
  Exec.cpp
  Instr.cpp
  Journ.cpp
  Level.cpp
  Limits.cpp
  Market.cpp
  MarketId.cpp
  MarketSide.cpp
  Model.cpp
  Msg.cpp
  MsgHandler.cpp
  MsgQueue.cpp
  Order.cpp
  Posn.cpp
  Request.cpp
  Transaction.cpp
  Types.cpp)

add_library(swirly-fin-static STATIC ${lib_SOURCES})
set_target_properties(swirly-fin-static PROPERTIES OUTPUT_NAME swirly-fin)
target_link_libraries(swirly-fin-static swirly-app-static)
install(TARGETS swirly-fin-static DESTINATION lib64 COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly-fin-shared SHARED ${lib_SOURCES})
  set_target_properties(swirly-fin-shared PROPERTIES OUTPUT_NAME swirly-fin)
  target_link_libraries(swirly-fin-shared swirly-app-shared)
  install(TARGETS swirly-fin-shared DESTINATION lib64 COMPONENT shared)
endif()

foreach(file ${lib_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.hpp")
    install(FILES "${name}.hpp"
      DESTINATION include/swirly/fin
      COMPONENT header
    )
  endif()
endforeach()

set(test_SOURCES
  Asset.ut.cpp
  BasicTypes.ut.cpp
  Instr.ut.cpp
  Date.ut.cpp
  Exception.ut.cpp
  Level.ut.cpp
  MarketId.ut.cpp
  Market.ut.cpp
  MsgHandler.ut.cpp
  MsgQueue.ut.cpp
  Posn.ut.cpp
  Request.ut.cpp
  Transaction.ut.cpp)

add_executable(swirly-fin-test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly-fin-test ${swirly_fin_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name "${file}" NAME_WE)
  add_test(NAME fin::${name}Suite COMMAND swirly-fin-test -l error -t ${name}Suite)
endforeach()
