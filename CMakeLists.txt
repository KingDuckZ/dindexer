cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(dindexer VERSION 0.1 LANGUAGES CXX C)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")

find_library(Boost REQUIRED 1.53.0)
find_package(PostgreSQL REQUIRED)
find_package(YamlCpp REQUIRED 0.5.1)

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
)

target_include_directories(${PROJECT_NAME} SYSTEM
	INTERFACE ${Boost_INCLUDE_DIR}
	PRIVATE ${PostgreSQL_INCLUDE_DIRS}
	PRIVATE ${YAMLCPP_INCLUDE_DIR}
)

target_include_directories(${PROJECT_NAME}
	PRIVATE src
	PRIVATE include
)

target_link_libraries(${PROJECT_NAME}
	${PostgreSQL_LIBRARIES}
	${YAMLCPP_LIBRARY}
)
