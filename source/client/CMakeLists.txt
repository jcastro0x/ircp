# This file is part of the IRCP Project. See LICENSE file for Copyright information

cmake_minimum_required(VERSION 3.20)
project(client)
set(CMAKE_CXX_STANDARD 23)

#
# Add unit files to server sources
# ----------------------------------------------------------------------------------------------------------------------
set(CLIENT_SOURCES
    client.cc
    client.rc
    razer/razer_chroma.cc
)

# Razer SDK files
list(APPEND CLIENT_SOURCES
    razer/ChromaAnimationAPI.cpp
    razer/ChromaSDKPluginTypes.cpp
    razer/RazerKeyboardMapping.cpp
    razer/VerifyLibrarySignature.cpp

    # replace ChromaLogger of razer to our own version
    razer/ChromaLogger_ircp.cpp # instead of 'razer/ChromaLogger.cpp'
)

#
# Create executable
# ----------------------------------------------------------------------------------------------------------------------
add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client SharedLibrary)

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/source/client)
target_compile_definitions(client PRIVATE -DUNICODE -D_UNICODE) # Required by Razer SDK

#
# Copy binaries
# ----------------------------------------------------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/source/client/binaries $<TARGET_FILE_DIR:${PROJECT_NAME}>)
