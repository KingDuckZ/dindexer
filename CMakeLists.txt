cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(dindexer VERSION 0.1.2 LANGUAGES CXX C)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
option(DINDEXER_DEBUG_CFG_FILE "Enable to set the config file path to the build path" OFF)
set(PROJECT_VERSION_BETA "1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")

if (DINDEXER_CONFIG_FILE STREQUAL "")
	if (DINDEXER_DEBUG_CFG_FILE)
		set(DINDEXER_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/dindexer.yml CACHE STRING "Path to the config file" FORCE)
	else()
		set(DINDEXER_CONFIG_FILE "$HOME/.config/dindexer.yml" CACHE STRING "Path to the config file" FORCE)
	endif()
endif()
message(STATUS "Config file set to ${DINDEXER_CONFIG_FILE}")

find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options)
find_package(PostgreSQL REQUIRED)
find_package(YamlCpp 0.5.1 REQUIRED)

configure_file(
	"${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}Config.h.in"
	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h"
)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/filesearcher.cpp
	src/pathname.cpp
	src/indexer.cpp
	src/tiger.c
	src/tiger.cpp
	src/pq/connection.cpp
	src/pq/databaseexception.cpp
	src/pq/resultset.cpp
	src/dbbackend.cpp
	src/settings.cpp
	src/commandline.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	INTERFACE ${Boost_INCLUDE_DIRS}
	PRIVATE ${PostgreSQL_INCLUDE_DIRS}
	PRIVATE ${YAMLCPP_INCLUDE_DIR}
)

target_include_directories(${PROJECT_NAME}
	PRIVATE src
	PRIVATE include
	PRIVATE ${PROJECT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
	${PostgreSQL_LIBRARIES}
	${YAMLCPP_LIBRARY}
	${Boost_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME}
	PUBLIC WITH_PROGRESS_FEEDBACK
)
