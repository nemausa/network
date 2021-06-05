cmake_minimum_required(VERSION 3.0)

STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ) 
PROJECT(${CURRENT_FOLDER})

add_definitions(-w)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

if (WIN32)
   message("-- This is Windows")
elseif(APPLE)
   message("-- This is Apple")
elseif(UNIX)
   message("-- This is UNIX")
endif()

add_subdirectory(EasyTcpServer)
add_subdirectory(EasyTcpClient)

FILE(GLOB SRC_LIST_CPP ${PROJECT_SOURCE_DIR}/*.cpp)

AUX_SOURCE_DIRECTORY(. SRC_LIST_CPP)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST_CPP})


set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/)
install(TARGETS ${PROJECT_NAME}
        EXPORT MyLibTargets 
        LIBRARY DESTINATION lib  # 动态库安装路径
        ARCHIVE DESTINATION lib  # 静态库安装路径
        RUNTIME DESTINATION bin  # 可执行文件安装路径
        PUBLIC_HEADER DESTINATION include  # 头文件安装路径
        )