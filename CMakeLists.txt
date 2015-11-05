cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(dindexer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/filesearcher.cpp
	src/pathname.cpp
)
