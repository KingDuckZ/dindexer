cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(bare_name "dindexer")
project("${bare_name}-if" VERSION 0.1.2 LANGUAGES CXX C)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
option(DINDEXER_DEBUG_CFG_FILE "Enable to set the config file path to the build path" OFF)
set(ACTIONS_PATH "${CMAKE_CURRENT_BINARY_DIR}/src" CACHE STRING "Actions search path")
set(PROJECT_VERSION_BETA "1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -Wall -Wextra")

if ("${DINDEXER_CONFIG_FILE}" STREQUAL "")
	if (DINDEXER_DEBUG_CFG_FILE)
		set(DINDEXER_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/${bare_name}.yml CACHE STRING "Path to the config file" FORCE)
	else()
		set(DINDEXER_CONFIG_FILE "$HOME/.config/${bare_name}.yml" CACHE STRING "Path to the config file" FORCE)
	endif()
endif()
message(STATUS "Config file set to \"${DINDEXER_CONFIG_FILE}\"")

find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options)
find_package(PostgreSQL REQUIRED)
find_package(YamlCpp 0.5.1 REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${bare_name}-inc INTERFACE)

message(STATUS "Actions search path set to: \"${ACTIONS_PATH}\"")

configure_file(
	"${PROJECT_SOURCE_DIR}/src/${bare_name}Config.h.in"
	"${PROJECT_BINARY_DIR}/${bare_name}Config.h"
)

target_include_directories(${PROJECT_NAME} SYSTEM
	INTERFACE ${Boost_INCLUDE_DIRS}
	INTERFACE ${PostgreSQL_INCLUDE_DIRS}
	INTERFACE ${YAMLCPP_INCLUDE_DIR}
)

target_include_directories(${bare_name}-inc
	INTERFACE ${PROJECT_BINARY_DIR}
	INTERFACE ${CMAKE_SOURCE_DIR}/include
)

add_subdirectory(src/scan)
add_subdirectory(src/pq)
add_subdirectory(src/main)

target_link_libraries(${PROJECT_NAME}
	INTERFACE ${PostgreSQL_LIBRARIES}
	INTERFACE ${YAMLCPP_LIBRARY}
	INTERFACE ${Boost_LIBRARIES}
	INTERFACE ${bare_name}-pq
	INTERFACE ${bare_name}-inc
)

target_compile_definitions(${PROJECT_NAME}
	INTERFACE WITH_PROGRESS_FEEDBACK
)
