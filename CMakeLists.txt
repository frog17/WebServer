# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("WebServer"
    VERSION 0.1.0
    LANGUAGES C CXX)

set(INC_DIR ${PROJECT_SOURCE_DIR}/_include CACHE PATH "include")

set(CXX_FLAGS
	-g
	-Wall
	-std=c++14
	-lpthread
)

add_custom_target(
	LINK_HEADERS ALL
	COMMENT "link headers..."
)

include(CMakeLists_Headers.txt)

macro(makeLink src dest target)
	add_custom_command(
		TARGET ${target} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src} ${dest}
		DEPENDS ${dest}
	)
endmacro()

add_custom_command(
	TARGET LINK_HEADERS PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${INC_DIR}/
)

foreach(header_file ${INCLUDE_HEADERS})
    string(REPLACE "/" ";" arr ${header_file})
	list(GET arr -1 file_name)
    makeLink(${PROJECT_SOURCE_DIR}/${header_file} ${INC_DIR}/${file_name} LINK_HEADERS)
endforeach()


include_directories(
	${CMAKE_SOURCE_DIR}/thirdparty
)

add_subdirectory(WebServer)
