cmake_minimum_required(VERSION 3.0)

string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ) 
project(${CURRENT_FOLDER})

add_definitions(-w -DDebug)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") 
   MESSAGE("-- Clang") 
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") 
   MESSAGE("-- GNU") 
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") 
   MESSAGE("-- Intel") 
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") 
   MESSAGE("-- MSVC") 
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -fpermissive")

if (WIN32)
   message("-- This is Windows")
elseif(APPLE)
   message("-- This is Apple")
elseif(UNIX)
   message("-- This is UNIX")
endif()

if(CMAKE_CL_64)    #CMAKE的内建变量，如果是true，就说明编译器的64位的，自然可以编译64bit的程序
   set(ADDRESS_MODEL 64)
   set(NODE_TARGET x64)
else()
   set(ADDRESS_MODEL 32)
   set(NODE_TARGET x86)
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/)

add_subdirectory(EasyTcpServer)
add_subdirectory(EasyTcpClient)
add_subdirectory(Pool)
