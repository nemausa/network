cmake_minimum_required(VERSION 3.0)

STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ) 
PROJECT(${CURRENT_FOLDER})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -pthread")




AUX_SOURCE_DIRECTORY(. SRC_LIST_CPP)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST_CPP} )
target_link_libraries(${PROJECT_NAME} ws2_32)

install(TARGETS ${PROJECT_NAME}
        EXPORT MyLibTargets 
        LIBRARY DESTINATION lib  # 动态库安装路径
        ARCHIVE DESTINATION lib  # 静态库安装路径
        RUNTIME DESTINATION bin  # 可执行文件安装路径
        PUBLIC_HEADER DESTINATION include  # 头文件安装路径
        )
