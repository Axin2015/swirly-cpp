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

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# FIXME: currently fails on some toolsets, so prefer QT5_ADD_RESOURCES for now.
#set(CMAKE_AUTORCC ON)
qt5_add_resources(prog_RESOURCES Application.qrc)

# Explicitly list sources to avoid accidentally including moc sources
# when build and source directories are the same.
set(prog_SOURCES
  Asset.cpp
  AssetModel.cpp
  AssetView.cpp
  Client.cpp
  Instr.cpp
  InstrModel.cpp
  InstrView.cpp
  Exec.cpp
  ExecModel.cpp
  ExecView.cpp
  HttpClient.cpp
  Level.cpp
  Main.cpp
  MainWindow.cpp
  Market.cpp
  MarketForm.cpp
  MarketModel.cpp
  MarketView.cpp
  Order.cpp
  OrderForm.cpp
  OrderModel.cpp
  OrderView.cpp
  Posn.cpp
  PosnModel.cpp
  PosnView.cpp
  Row.cpp
  TableModel.cpp
  TradeModel.cpp
  TradeView.cpp
  Types.cpp
  Utility.cpp)

add_executable(swirlyui ${prog_RESOURCES} ${prog_SOURCES})
target_link_libraries(swirlyui Qt5::Widgets Qt5::Network)
install(TARGETS swirlyui DESTINATION bin COMPONENT program)
