cmake_minimum_required(VERSION 3.10)
project (network)

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

IF (WIN32)
	message(status " Windows")
	set (OS "WIN32")
   	set (OS_CXX_FLAGS "")
	link_directories(
		lib/windows
	)
macro(get_WIN32_WINNT version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})
        set(${version} "0x${ver}")
    endif()
endmacro()

get_WIN32_WINNT(ver)
add_definitions(-D_WIN32_WINNT=${ver})
ELSEIF (APPLE)
	message(status " Apple")
    link_directories(
        lib/osx
    )
ELSEIF (UNIX)
	message(status " Linux")
    set (OS "LINUX")
    set (OS_CXX_FLAGS "")
	link_directories(
		lib/unix
	)
ENDIF ()

include_directories(
	include
)
set(OS_CXX_FLAGS "${OS_CXX_FLAGS} -fpermissive -g -std=c++11 -pthread -O -Wreturn-type")

if (MSVC)
	message(using MSVC compiler)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OS_CXX_FLAGS}")
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/)


# ---------------------------------------------------------------------------------------
# Example of using pre-compiled library
# ---------------------------------------------------------------------------------------

# add_subdirectory(tcp_server)
# add_subdirectory(tcp_client)
# add_subdirectory(http_server)
# add_subdirectory(http_client)
add_subdirectory(websocket_server)
