cmake_minimum_required(VERSION 3.0)

string( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ) 
project(${CURRENT_FOLDER})

aux_source_directory(. SRC_LIST_CPP)
add_executable(${PROJECT_NAME} ${SRC_LIST_CPP} )

if (WIN32)
target_link_libraries(${PROJECT_NAME} ws2_32)
endif()




install(TARGETS ${PROJECT_NAME}
        EXPORT MyLibTargets 
        LIBRARY DESTINATION lib  # 动态库安装路径
        ARCHIVE DESTINATION lib  # 静态库安装路径
        RUNTIME DESTINATION bin  # 可执行文件安装路径
        PUBLIC_HEADER DESTINATION include  # 头文件安装路径
        )
