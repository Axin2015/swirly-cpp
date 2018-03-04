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
  add_custom_target(swirly_fin DEPENDS
    swirly_fin_static
    swirly_fin_shared
    swirly_fin_test
  )
else()
  add_custom_target(swirly_fin DEPENDS
    swirly_fin_static
    swirly_fin_test
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
  Order.cpp
  Posn.cpp
  Request.cpp
  Transaction.cpp
  Types.cpp)

add_library(swirly_fin_static STATIC ${lib_SOURCES})
set_target_properties(swirly_fin_static PROPERTIES OUTPUT_NAME swirly_fin)
target_link_libraries(swirly_fin_static swirly_util_static)
install(TARGETS swirly_fin_static DESTINATION lib COMPONENT static)

if(ENABLE_SHARED_LIBS)
  add_library(swirly_fin_shared SHARED ${lib_SOURCES})
  set_target_properties(swirly_fin_shared PROPERTIES OUTPUT_NAME swirly_fin)
  target_link_libraries(swirly_fin_shared swirly_util_shared)
  install(TARGETS swirly_fin_shared DESTINATION lib COMPONENT shared)
endif()

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
  Posn.ut.cpp
  Request.ut.cpp
  Transaction.ut.cpp)

add_executable(swirly_fin_test
  ${test_SOURCES}
  Main.ut.cpp)
target_link_libraries(swirly_fin_test ${swirly_fin_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

foreach(file ${test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  add_test(NAME fin::${name}Suite COMMAND swirly_fin_test -l error -t ${name}Suite)
endforeach()
