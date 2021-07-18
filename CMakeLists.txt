cmake_minimum_required(VERSION 2.8.12)
project (network)

set (SERVER_NAME server)
set (CLIENT_NAME client)
set (POOL_NAME pool)

set (NETWORK_MAJOR_VERSION 0)
set (NETWORK_MINOR_VERSION 0)
set (NETWORK_PATCH_VERSION 0)
set (NETWORK_HOTIFX_VERSION 0)

set (NETWORK_VERSION ${NETWORK_MAJOR_VERSION}.${NETWORK_MINOR_VERSION}.${NETWORK_PATCH_VERSION}.${NETWORK_HOTIFX_VERSION})
set (CMAKE_VERBOSE_MAKEFILE off)

# Set a default build type if none was specified
set (default_build_type "RelWithDebInfo")
if(NOT CMAKE_BUILD_TYPE)
   message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
   set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
   # Set the possible values of build type for cmake-gui
   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#OS
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	message(status "Linux")
    set (OS "LINUX")
    set (OS_CXX_FLAGS "")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "WIN32")
	message(status "Windows")
	set (OS "WIN32")
   	set (OS_CXX_FLAGS "")
endif (${CMAKE_SYSTEM_NAME} MATCHES "WIN32")


################################################################################
# Options
################################################################################



################################################################################
# Directories
################################################################################

include_directories(
	depends/include
)

if (MSVC)
	message(using MSVC compiler)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OS_CXX_FLAGS} -g -std=c++11 -pthread")
endif()


################################################################################
# server executable
################################################################################
file(GLOB ${SERVER_NAME}_SRC
	"depends/src/*.cpp"
	"server/src/*.cpp"
)
message (status "${${SERVER_NAME}_SRC}")
list (SORT ${SERVER_NAME}_SRC)

add_executable(${SERVER_NAME} ${${SERVER_NAME}_SRC} )
if (WIN32)
	target_link_libraries(${SERVER_NAME} ws2_32)
endif()

################################################################################
# client executable
################################################################################
file(GLOB ${CLIENT_NAME}_SRC
	"depends/src/*.cpp"
	"client/src/*.cpp"
)
message (status "${${CLIENT_NAME}_SRC}")
list (SORT ${CLIENT_NAME}_SRC)

add_executable(${CLIENT_NAME} ${${CLIENT_NAME}_SRC} )
if (WIN32)
	target_link_libraries(${CLIENT_NAME} ws2_32)
endif()

################################################################################
# server executable
################################################################################
file(GLOB ${POOL_NAME}_SRC
	"depends/src/*.cpp"
	"pool/src/*.cpp"
)
message (status "${${POOL_NAME}_SRC}")
list (SORT ${POOL_NAME}_SRC)

add_executable(${POOL_NAME} ${${POOL_NAME}_SRC} )
if (WIN32)
	target_link_libraries(${POOL_NAME} ws2_32)
endif()

################################################################################
# Installation
################################################################################
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/)

install(TARGETS ${SERVER_NAME}
	LIBRARY DESTINATION lib  # 动态库安装路径
	ARCHIVE DESTINATION lib  # 静态库安装路径
	RUNTIME DESTINATION bin  # 可执行文件安装路径
	PUBLIC_HEADER DESTINATION include  # 头文件安装路径
)

install(TARGETS ${CLIENT_NAME}
	LIBRARY DESTINATION lib  # 动态库安装路径
	ARCHIVE DESTINATION lib  # 静态库安装路径
	RUNTIME DESTINATION bin  # 可执行文件安装路径
	PUBLIC_HEADER DESTINATION include  # 头文件安装路径
)

install(TARGETS ${POOL_NAME}
	LIBRARY DESTINATION lib  # 动态库安装路径
	ARCHIVE DESTINATION lib  # 静态库安装路径
	RUNTIME DESTINATION bin  # 可执行文件安装路径
	PUBLIC_HEADER DESTINATION include  # 头文件安装路径
)