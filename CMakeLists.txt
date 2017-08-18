cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
set(bare_name "dindexer")
project("${bare_name}-if" VERSION 0.1.5 LANGUAGES CXX C)
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A file indexing program to help you keep track of your backed up files")
set(CPACK_PACKAGE_VENDOR "King_DuckZ")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_GENERATOR "DEB;RPM;TGZ")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "King_DuckZ")
set(CPACK_PACKAGE_NAME "${bare_name}")
set(CPACK_STRIP_FILES ON)

#Make sure this is either Debug or Release
set(DINDEXER_SUPPORTED_BUILD_TYPES "Debug;Release")
list(FIND DINDEXER_SUPPORTED_BUILD_TYPES "${CMAKE_BUILD_TYPE}" build_type_index)
if (${build_type_index} LESS 0)
	message(STATUS "Forcing build type to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()
unset(build_type_index)

include(GetGitRevisionDescription)
include(Buildlibpqtypes)
include(gccversion)
include(CPack)
include(CTest)
include(timestamp)
include(shared_git_project)

option(DINDEXER_DEBUG_CFG_FILE "Enable to set the config file path to the build path" OFF)
option(DINDEXER_WITH_MEDIA_AUTODETECT "Enable code that tries to autodetect the media type and sets --type automatically" ON)
option(DINDEXER_NATIVE_RELEASE "Pass the -march=native flag to the compiler for release builds" OFF)
if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION_MAJOR EQUAL "5")
	option(DINDEXER_CXX11_ABI "Controls if _GLIBCXX_USE_CXX11_ABI gets set to 0 or not" ON)
endif()
if(DINDEXER_NATIVE_RELEASE)
	set(march_flag "-march=native")
else()
	set(march_flag "")
endif()
set(DINDEXER_COPYRIGHT_YEARS "2015,2016")
set(DINDEXER_ACTIONS_PATH "${CMAKE_CURRENT_BINARY_DIR}/src" CACHE STRING "Actions search path")
string(REGEX MATCH "[^/].*" ACTIONS_PATH_INSTALL "${DINDEXER_ACTIONS_PATH}")
set(DINDEXER_DB_OWNER_NAME "$ENV{USER}" CACHE STRING "Name that will be used as the DB owner name")
set(PROJECT_VERSION_BETA "1")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wno-missing-field-initializers -fno-omit-frame-pointer -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -fomit-frame-pointer -Wno-missing-field-initializers ${march_flag}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -fno-omit-frame-pointer -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -fomit-frame-pointer ${march_flag}")

set(DINDEXER_PUB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PBL_WITH_TESTS OFF)
get_git_head_revision(GIT_REFSPEC PROJECT_VERSION_GIT)

if ("${DINDEXER_CONFIG_FILE}" STREQUAL "")
	if (DINDEXER_DEBUG_CFG_FILE)
		set(DINDEXER_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/${bare_name}.yml CACHE STRING "Path to the config file" FORCE)
	else()
		set(DINDEXER_CONFIG_FILE "$HOME/.config/${bare_name}.yml" CACHE STRING "Path to the config file" FORCE)
	endif()
endif()
message(STATUS "Config file set to \"${DINDEXER_CONFIG_FILE}\"")

find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options filesystem system)
find_package(PostgreSQL 8.3 REQUIRED)
find_package(YamlCpp 0.5.1 REQUIRED)
import_libpqtypes_project("${PostgreSQL_INCLUDE_DIRS}" "-O3 ${march_flag}")

add_library(${PROJECT_NAME} INTERFACE)
add_library(${bare_name}-inc INTERFACE)

message(STATUS "Actions search path set to: \"${DINDEXER_ACTIONS_PATH}\"")

configure_file(
	"${PROJECT_SOURCE_DIR}/src/${bare_name}Config.h.in"
	"${PROJECT_BINARY_DIR}/${bare_name}Config.h"
)
configure_file(
	"${PROJECT_SOURCE_DIR}/src/gitinfo.h.in"
	"${PROJECT_BINARY_DIR}/gitinfo.h"
)
configure_file(
	"${PROJECT_SOURCE_DIR}/dindexer.sql.in"
	"${PROJECT_BINARY_DIR}/dindexer.sql"
)
configure_file(
	"${PROJECT_SOURCE_DIR}/dindexer_cmd_complete.sh.in"
	"${PROJECT_BINARY_DIR}/dindexer_cmd_complete.sh"
	@ONLY
)

target_include_directories(${PROJECT_NAME} SYSTEM
	INTERFACE ${Boost_INCLUDE_DIRS}
)

target_compile_features(${PROJECT_NAME}
	INTERFACE cxx_nullptr
	INTERFACE cxx_range_for
	INTERFACE cxx_lambdas
	INTERFACE cxx_decltype_auto
	INTERFACE cxx_defaulted_functions
	INTERFACE cxx_deleted_functions
	INTERFACE cxx_auto_type
	INTERFACE cxx_decltype_incomplete_return_types
	INTERFACE cxx_defaulted_move_initializers
	INTERFACE cxx_noexcept
	INTERFACE cxx_rvalue_references
)

target_include_directories(${bare_name}-inc
	INTERFACE ${PROJECT_BINARY_DIR}
	INTERFACE ${CMAKE_SOURCE_DIR}/include
)

if (NOT DINDEXER_CXX11_ABI AND CMAKE_CXX_COMPILER_VERSION_MAJOR EQUAL "5")
	message(STATUS "CXX11 ABI disabled (_GLIBCXX_USE_CXX11_ABI=0)")
	add_definitions(
		#workaround for a bug in gcc 5.3 that is causing exceptions to slip
		#through try/catch blocks.
		#WARNING: this will likely cause linking erros with boost and
		#yaml-cpp unless they are also built with this option or with an
		#older version of gcc.
		-D_GLIBCXX_USE_CXX11_ABI=0
	)
endif()

string(REPLACE "," ";" backend_list "${DINDEXER_ENABLED_BACKENDS}")
list(FIND backend_list "redis" redis_found_index)
if (${redis_found_index} GREATER -1)
	set(DINDEXER_WITH_REDIS ON)
endif()
unset(redis_found_index)
unset(backend_list)

#Libraries
add_subdirectory(src/pq)
add_subdirectory(src/common)
add_subdirectory(src/machinery)
add_subdirectory(lib/pbl)
add_subdirectory(lib/glob2regex)
add_subdirectory(lib/SQLiteCpp EXCLUDE_FROM_ALL)
add_subdirectory(src/backends)
add_subdirectory(src/core)
add_shared_git_project(lib/duckhandy)
if (DINDEXER_WITH_REDIS)
	add_shared_git_project(lib/incredis EXCLUDE_FROM_ALL)
endif()

#Actions
add_subdirectory(src/main)
add_subdirectory(src/scan)
add_subdirectory(src/delete)
#add_subdirectory(src/query)
add_subdirectory(src/locate)
add_subdirectory(src/navigate)
add_subdirectory(src/tag)

#Tests
if (BUILD_TESTING)
	set(TMP_PROJECT_VERSION "${PROJECT_VERSION}")
	set(TMP_PROJECT_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
	set(TMP_PROJECT_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
	set(TMP_PROJECT_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
	unset(PROJECT_VERSION)
	unset(PROJECT_VERSION_MAJOR)
	unset(PROJECT_VERSION_MINOR)
	unset(PROJECT_VERSION_PATCH)
	add_shared_git_project(test/gtest)
	set(PROJECT_VERSION "${TMP_PROJECT_VERSION}")
	set(PROJECT_VERSION_MAJOR "${TMP_PROJECT_VERSION_MAJOR}")
	set(PROJECT_VERSION_MINOR "${TMP_PROJECT_VERSION_MINOR}")
	set(PROJECT_VERSION_PATCH "${TMP_PROJECT_VERSION_PATCH}")
	add_subdirectory(test/unit)
	add_subdirectory(test/unit_cli)
endif()

target_link_libraries(${PROJECT_NAME}
	INTERFACE ${Boost_LIBRARIES}
	INTERFACE ${bare_name}-pq
	INTERFACE ${bare_name}-inc
)
target_link_libraries(${bare_name}-inc
	INTERFACE duckhandy
)

target_compile_definitions(${PROJECT_NAME}
	INTERFACE WITH_PROGRESS_FEEDBACK
	INTERFACE BOOST_SPIRIT_USE_PHOENIX_V3=1
)
