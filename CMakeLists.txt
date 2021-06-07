cmake_minimum_required(VERSION 3.0)

string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ) 
project(${CURRENT_FOLDER})

add_definitions(-w)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

if (WIN32)
   message("-- This is Windows")
elseif(APPLE)
   message("-- This is Apple")
elseif(UNIX)
   message("-- This is UNIX")
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/)

add_subdirectory(EasyTcpServer)
add_subdirectory(EasyTcpClient)
