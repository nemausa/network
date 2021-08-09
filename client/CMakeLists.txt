cmake_minimum_required(VERSION 3.10)

string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ) 
project(${CURRENT_FOLDER} CXX)

file(GLOB ${PROJECT_NAME}_SRC
	"../implementation/depends/src/*.cpp"
	"../implementation/config/src/*.cpp"
	"src/*.cpp"
)

list (SORT ${PROJECT_NAME}_SRC)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC} )

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message(status "Windows")
	target_link_libraries(${PROJECT_NAME} ws2_32)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# if (NOT TARGET spdlog)
#     # Stand-alone build
#     find_package(spdlog REQUIRED)
# 	message(status " spdlog")
# endif ()
# target_link_libraries(${PROJECT_NAME} glog)
target_link_libraries(${PROJECT_NAME} spdlog)
install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib  # 动态库安装路径
	ARCHIVE DESTINATION lib  # 静态库安装路径
	RUNTIME DESTINATION bin  # 可执行文件安装路径
	PUBLIC_HEADER DESTINATION include  # 头文件安装路径
)