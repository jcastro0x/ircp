# This file is part of the IRCP Project. See LICENSE file for Copyright information

cmake_minimum_required(VERSION 3.20)
project(IRCP)

##
## C++23 support
## ----------------------------------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##
## Shared include directories
## ----------------------------------------------------------------------------------------------------------------------
include_directories(source)

#
# SharedLibrary
# ----------------------------------------------------------------------------------------------------------------------
set(SHARED_LIBRARY_SOURCES
    source/shared/network/socket.cc
    source/shared/network/package_global.cc
    source/shared/config/config.cc
    source/shared/logging/logging_boost.cc
)

add_library(SharedLibrary STATIC ${SHARED_LIBRARY_SOURCES})
target_precompile_headers(SharedLibrary PRIVATE source/shared/pch.hh)

#
# Add and Link Boost Libraries
# ----------------------------------------------------------------------------------------------------------------------
find_package(Boost 1.83 REQUIRED COMPONENTS program_options log locale) # header-only used too: { asio, crc, property_tree }
target_link_libraries(SharedLibrary ${Boost_LIBRARIES})
include_directories(SharedLibrary PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_definitions(SharedLibrary PRIVATE BOOST_ASIO_NO_DEPRECATED)

#
# Add compiler warning level options
# ----------------------------------------------------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
endif()

#
# Specific configuration to MS Windows
# ----------------------------------------------------------------------------------------------------------------------
if(WIN32)
    # Force the WinSock version
    # https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
    add_definitions(-D_WIN32_WINNT=0x0601) # 0x0601 = Windows 7
    add_definitions(-D_CRT_SECURE_NO_WARNINGS) #strcpy_s and family doesn't exists in gcc nor clang
endif()

#
# Server
# ----------------------------------------------------------------------------------------------------------------------
add_subdirectory(source/server)

add_subdirectory(test)

#
# Client
# ----------------------------------------------------------------------------------------------------------------------
if(WIN32)
    add_subdirectory(source/client)
endif()
