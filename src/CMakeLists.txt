# ****************************************************************************
#  Copyright (C) 2015 Karol Roslaniec <mariacpp@roslaniec.net>
# 
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not see <http://www.gnu.org/licenses>
#  or write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ****************************************************************************

FILE(GLOB SRC *.cpp *.c)
INCLUDE_DIRECTORIES(${MARIADB_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
ADD_LIBRARY(mariacpp STATIC ${SRC})

# windows clang-cl support
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND "x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC" OR MSVC)
    target_compile_options(mariacpp PUBLIC /EHa /Zc:__cplusplus /MTd -m64)
    add_link_options(/MTd)
    add_compile_definitions(WIN32)
endif()

INSTALL(TARGETS mariacpp
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
