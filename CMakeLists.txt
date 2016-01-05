cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(bare_name "dindexer")
project("${bare_name}-if" VERSION 0.1.3 LANGUAGES CXX C)
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

include(GetGitRevisionDescription)
include(Buildlibpqtypes)

option(DINDEXER_DEBUG_CFG_FILE "Enable to set the config file path to the build path" OFF)
option(DINDEXER_WITH_MEDIA_AUTODETECT "Enable code that tries to autodetect the media type and sets --type automatically" ON)
set(ACTIONS_PATH "${CMAKE_CURRENT_BINARY_DIR}/src" CACHE STRING "Actions search path")
set(PROJECT_VERSION_BETA "1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -Wall -Wextra -Wno-missing-field-initializers")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -Wall -Wextra -O3 -fomit-frame-pointer -Wno-missing-field-initializers")
set(DINDEXER_PUB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
get_git_head_revision(GIT_REFSPEC PROJECT_VERSION_GIT)

if ("${DINDEXER_CONFIG_FILE}" STREQUAL "")
	if (DINDEXER_DEBUG_CFG_FILE)
		set(DINDEXER_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/${bare_name}.yml CACHE STRING "Path to the config file" FORCE)
	else()
		set(DINDEXER_CONFIG_FILE "$HOME/.config/${bare_name}.yml" CACHE STRING "Path to the config file" FORCE)
	endif()
endif()
message(STATUS "Config file set to \"${DINDEXER_CONFIG_FILE}\"")

find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options)
find_package(PostgreSQL 8.3 REQUIRED)
find_package(YamlCpp 0.5.1 REQUIRED)
import_libpqtypes_project("${PostgreSQL_INCLUDE_DIRS}")

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
)

target_include_directories(${bare_name}-inc
	INTERFACE ${PROJECT_BINARY_DIR}
	INTERFACE ${CMAKE_SOURCE_DIR}/include
)

#Libraries
add_subdirectory(src/pq)
add_subdirectory(src/common)
add_subdirectory(src/machinery)

#Actions
add_subdirectory(src/main)
add_subdirectory(src/scan)
add_subdirectory(src/delete)
add_subdirectory(src/query)
add_subdirectory(src/locate)

target_link_libraries(${PROJECT_NAME}
	INTERFACE ${PostgreSQL_LIBRARIES}
	INTERFACE ${Boost_LIBRARIES}
	INTERFACE ${bare_name}-pq
	INTERFACE ${bare_name}-inc
)

target_compile_definitions(${PROJECT_NAME}
	INTERFACE WITH_PROGRESS_FEEDBACK
	INTERFACE BOOST_SPIRIT_USE_PHOENIX_V3=1
)
