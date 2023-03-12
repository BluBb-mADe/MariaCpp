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

MESSAGE("-- ${CMAKE_BUILD_TYPE} build")

# windows clang-cl
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
    MESSAGE("-- Compiler: clang-cl")
    target_compile_options(mariacpp PUBLIC /EHa /MT$<$<CONFIG:Debug>:d>)
    add_compile_definitions(WIN32)
endif()

# msvc
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    MESSAGE("-- Compiler: MSVC")

    set(CompilerFlags CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MDd?" "" ${CompilerFlag} "${${CompilerFlag}}")
        set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
    endforeach()
    string(REGEX REPLACE "/EHs?c?" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    target_compile_options(mariacpp PUBLIC /EHa /Zc:__cplusplus /MT$<$<CONFIG:Debug>:d>)
endif()

set_target_properties(mariacpp PROPERTIES DEBUG_POSTFIX d)

if(DEFINED ASAN)
    MESSAGE("-- address sanitizer enabled")
    get_target_property(suffix mariacpp DEBUG_POSTFIX)
    set_target_properties(mariacpp PROPERTIES DEBUG_POSTFIX ${suffix}_as)
    target_compile_options(mariacpp PUBLIC /fsanitize=address)
    add_compile_definitions(_DISABLE_VECTOR_ANNOTATION)
    add_compile_definitions(_DISABLE_STRING_ANNOTATION)
    set(ignoreMe "${ASAN}")
endif()

INSTALL(TARGETS mariacpp
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
