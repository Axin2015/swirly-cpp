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

add_custom_target(swirly-tool DEPENDS
  swirly-db-to-dsv
  swirly-db-to-json
  swirly-hdr-decoder
  swirly-hiccup
)

add_executable(swirly-db-to-dsv DbToDsv.cpp)
target_link_libraries(swirly-db-to-dsv ${swirly_db_LIBRARY})
install(TARGETS swirly-db-to-dsv DESTINATION bin COMPONENT program)

add_executable(swirly-db-to-json DbToJson.cpp)
target_link_libraries(swirly-db-to-json ${swirly_db_LIBRARY})
install(TARGETS swirly-db-to-json DESTINATION bin COMPONENT program)

add_executable(swirly-hdr-decoder HdrDecoder.cpp)
target_link_libraries(swirly-hdr-decoder ${swirly_hdr_LIBRARY} ${swirly_sys_LIBRARY})
install(TARGETS swirly-hdr-decoder DESTINATION bin COMPONENT program)

add_executable(swirly-hiccup Hiccup.cpp)
target_link_libraries(swirly-hiccup ${swirly_hdr_LIBRARY} ${swirly_sys_LIBRARY})
install(TARGETS swirly-hiccup DESTINATION bin COMPONENT program)
