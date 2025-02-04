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

CMAKE_MINIMUM_REQUIRED (VERSION 3.25)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

PROJECT(mariacpp)

set_property(GLOBAL PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

find_library(MARIADB_LIBRARY
        NAMES libmariadb mariadb
        PATH_SUFFIXES lib mysql mariadb
        REQUIRED)

MESSAGE("-- Found mariadb library: ${MARIADB_LIBRARY}")

find_path(MARIADB_INCLUDE_DIR
    mysql.h
    PATH_SUFFIXES .. include mysql mariadb
    REQUIRED)

MESSAGE("-- Found mariadb include: ${MARIADB_INCLUDE_DIR}")

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

add_subdirectory(src)
if(CMAKE_PROJECT_NAME STREQUAL mariacpp AND BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
add_subdirectory(mariacpp)
