# This file is part of the IRCP Project. See LICENSE file for Copyright information

cmake_minimum_required(VERSION 3.20)
project(server)
set(CMAKE_CXX_STANDARD 23)

#
# Add unit files to server sources
# ----------------------------------------------------------------------------------------------------------------------
set(SERVER_SOURCES
    server.cc
)

if(WIN32)
    list(APPEND SERVER_SOURCES server.rc)
endif()

#
# Create executable
# ----------------------------------------------------------------------------------------------------------------------
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server ${SHARED_LIBRARY_NAME})

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/source/server)

#
# Copy binaries
# ----------------------------------------------------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/source/server/binaries $<TARGET_FILE_DIR:${PROJECT_NAME}>)
