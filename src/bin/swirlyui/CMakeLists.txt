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

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# FIXME: currently fails on some toolsets, so prefer QT5_ADD_RESOURCES for now.
#set(CMAKE_AUTORCC ON)
qt5_add_resources(swirlyui_RESOURCES Application.qrc)

file(GLOB swirlyui_SOURCES "*.cpp")

add_executable(swirlyui ${swirlyui_RESOURCES} ${swirlyui_SOURCES})
target_link_libraries(swirlyui Qt5::Widgets Qt5::Network)
