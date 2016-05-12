cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(glob2regex VERSION 0.1.0 LANGUAGES CXX)

find_package(Boost 1.53.0 REQUIRED)

add_library(${PROJECT_NAME}
	src/glob2regex.cpp
	src/glob_ast.cpp
	src/render_ast.cpp
	src/exceptions.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)
target_include_directories(${PROJECT_NAME}
	PRIVATE src
	PUBLIC include
)

target_compile_features(${PROJECT_NAME}
	PUBLIC cxx_range_for
	PUBLIC cxx_defaulted_functions
	PUBLIC cxx_auto_type
	PUBLIC cxx_noexcept
	PUBLIC cxx_rvalue_references
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE BOOST_SPIRIT_USE_PHOENIX_V3=1
)
