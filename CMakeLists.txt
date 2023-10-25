# This file is part of the IRCP Project. See LICENSE file for Copyright information

cmake_minimum_required(VERSION 3.20)
project(IRCP)

#
# C++23 support
# ----------------------------------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#
# Check compilers versions
# ----------------------------------------------------------------------------------------------------------------------
# In this project, we utilize the latest features of C++20 and C++23. As such, only the most recent versions of the
# supported compilers include these features. This checker allows informing the programmer if they have the latest
# minimum versions supporting such features installed.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13.0)
    message(FATAL_ERROR "You need GCC version 13.0 or later")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0)
    message(FATAL_ERROR "You need Clang version 14.0 or later")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16.11.14)
    message(FATAL_ERROR "You need MSVC version 16.11.14 or later")
endif()

#
# Shared include directories
# ----------------------------------------------------------------------------------------------------------------------
include_directories(source)

#
# SharedLibrary
# ----------------------------------------------------------------------------------------------------------------------
set(SHARED_LIBRARY_NAME SharedLibrary)

set(SHARED_LIBRARY_SOURCES
    source/shared/network/socket.cc
    source/shared/network/package_global.cc
    source/shared/config/config.cc
    source/shared/logging/logging_boost.cc
)

add_library(${SHARED_LIBRARY_NAME} STATIC ${SHARED_LIBRARY_SOURCES})
target_precompile_headers(${SHARED_LIBRARY_NAME} PRIVATE source/shared/pch.hh)

#
# Add and Link OpenSSL Libraries
# ----------------------------------------------------------------------------------------------------------------------
find_package(OpenSSL REQUIRED)
target_link_libraries(${SHARED_LIBRARY_NAME}
    OpenSSL::SSL
    OpenSSL::Crypto
)

#
# Add and Link Boost Libraries
# ----------------------------------------------------------------------------------------------------------------------
find_package(Boost 1.83 REQUIRED COMPONENTS
    program_options
    log
    locale
    # header-only used too:
    #   asio
    #   crc
    #   property_tree
    #   ssl
)
target_link_libraries(${SHARED_LIBRARY_NAME} ${Boost_LIBRARIES})
include_directories(${SHARED_LIBRARY_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_definitions(${SHARED_LIBRARY_NAME} PRIVATE BOOST_ASIO_NO_DEPRECATED)

#
# Add compiler warning level options
# ----------------------------------------------------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -pedantic #[[-Werror]])
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 #[[ /WX]])
else()
    message(WARNING "Unknown compiler. Please verify the necessary compiler options.")
endif()

#
# Specific configuration to MS Windows
# ----------------------------------------------------------------------------------------------------------------------
if(WIN32)
    # Force the WinSock version
    # https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
    add_definitions(-D_WIN32_WINNT=0x0601) # 0x0601 = Windows 7
    add_definitions(-D_CRT_SECURE_NO_WARNINGS) # strcpy_s and its related functions do not exist in both gcc and clang.
endif()

#
# Server
# ----------------------------------------------------------------------------------------------------------------------
add_subdirectory(source/server)

#
# Tests
# ----------------------------------------------------------------------------------------------------------------------
add_subdirectory(test)

#
# Client
# ----------------------------------------------------------------------------------------------------------------------
if(WIN32)
    add_subdirectory(source/client)
endif()
