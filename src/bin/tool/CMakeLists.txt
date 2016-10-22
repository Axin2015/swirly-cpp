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

install(PROGRAMS ${bin_FILES} DESTINATION bin)

add_executable(swirly_bench Bench.cpp)
target_link_libraries(swirly_bench ${fig_LIBRARY} ${sqlite_LIBRARY})
install(TARGETS swirly_bench DESTINATION bin)

add_executable(swirly_dump Dump.cpp)
target_link_libraries(swirly_dump ${sqlite_LIBRARY})
install(TARGETS swirly_dump DESTINATION bin)

add_executable(swirly_httpd Httpd.cpp)
target_link_libraries(swirly_httpd
  ${fir_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} pthread)
install(TARGETS swirly_httpd DESTINATION bin)
