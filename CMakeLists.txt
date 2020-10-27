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

CMAKE_MINIMUM_REQUIRED (VERSION 3.9)
PROJECT(mariacpp)
SET(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

find_path(MARIADB_INCLUDE_DIR
    mysql.h
    PATHS "${MARIADB_PATH}"
    PATH_SUFFIXES mariadb include
    REQUIRED)

MESSAGE("-- Found mariadb include: ${MARIADB_INCLUDE_DIR}")

find_library(MARIADB_LIBRARY
    NAMES libmariadb mariadb
    PATHS "${MARIADB_PATH}"
    REQUIRED)

MESSAGE("-- Found mariadb library: ${MARIADB_LIBRARY}")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
if (CMAKE_USE_PTHREADS_INIT)
  set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result AND CMAKE_BUILD_TYPE STREQUAL "Release")
  MESSAGE("-- lto enabled")
  set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

enable_testing()
add_subdirectory(mariacpp)
add_subdirectory(src)
add_subdirectory(test)
