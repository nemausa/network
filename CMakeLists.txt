cmake_minimum_required(VERSION 3.0)

SET(CMAKE_CXX_COMPILER "D:/Tool/x86_64-8.1.0-release-posix-seh-rt_v6-rev0/mingw64/bin/g++.exe")

STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ) 
PROJECT(${CURRENT_FOLDER})

add_definitions(-w)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++11 -pthread")

if (WIN32)
    message("-- This is Windows")
elseif(APPLE)
    message("-- This is Apple")
elseif(UNIX)
    message("-- This is Linux")
endif()


add_subdirectory(EasyTcpServer)

AUX_SOURCE_DIRECTORY(. SRC_LIST_CPP)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST_CPP})
