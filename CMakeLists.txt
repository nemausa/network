cmake_minimum_required(VERSION 2.8.12)

string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ) 
project(${CURRENT_FOLDER})

# set(Glog_DIR "/usr/local/lib/cmake/glog")
find_package(glog 0.7.0 REQUIRED)
# link_directories("/usr/local/lib/")

file(GLOB ${PROJECT_NAME}_SRC
	"*.cpp"
)

list (SORT ${PROJECT_NAME}_SRC)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC} )


if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message(status " Windows")
	target_link_libraries(${PROJECT_NAME} ws2_32)
	target_link_libraries(${PROJECT_NAME} iphlpapi)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

target_link_libraries(${PROJECT_NAME} glog::glog)


set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib  # 动态库安装路径
	ARCHIVE DESTINATION lib  # 静态库安装路径
	RUNTIME DESTINATION bin  # 可执行文件安装路径
	PUBLIC_HEADER DESTINATION include  # 头文件安装路径
)


# export LD_LIBRARY_PATH=/usr/local/lib