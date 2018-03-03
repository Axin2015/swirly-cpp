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
  )
else()
  add_custom_target(swirly_fin DEPENDS
    swirly_fin_static
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
  AssetTest.cc
  BasicTypesTest.cc
  InstrTest.cc
  DateTest.cc
  ExceptionTest.cc
  LevelTest.cc
  MarketIdTest.cc
  MarketTest.cc
  MsgHandlerTest.cc
  PosnTest.cc
  RequestTest.cc
  TransactionTest.cc)

foreach(file ${test_SOURCES})
  get_filename_component (name ${file} NAME_WE)
  set(name "swirly_fin_${name}")
  add_executable(${name} ${file})
  target_link_libraries(${name} ${swirly_fin_LIBRARY} ${swirly_unit_LIBRARY})
  add_test(NAME ${name} COMMAND ${name})
endforeach()
