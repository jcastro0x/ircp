# This file is part of the IRCP Project. See LICENSE file for Copyright information

cmake_minimum_required(VERSION 3.20)
project(test)
set(CMAKE_CXX_STANDARD 23)

#
# Add unit files to server sources
# ----------------------------------------------------------------------------------------------------------------------
set(TEST_SOURCES
    example.cc
)

#
# Create executable
# ----------------------------------------------------------------------------------------------------------------------
add_executable(test ${TEST_SOURCES})
target_link_libraries(test SharedLibrary)

#
# Add and Link Boost Libraries
# ----------------------------------------------------------------------------------------------------------------------
find_package(Boost 1.83 REQUIRED COMPONENTS unit_test_framework)
target_link_libraries(test ${Boost_LIBRARIES})

add_test(NAME test_test COMMAND test)
