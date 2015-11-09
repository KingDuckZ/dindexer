cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(dindexer VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")

find_library (Boost REQUIRED 1.53.0)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/filesearcher.cpp
	src/pathname.cpp
	src/indexer.cpp
	src/tiger.c
	src/tiger.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	INTERFACE ${Boost_INCLUDE_DIR}
)
