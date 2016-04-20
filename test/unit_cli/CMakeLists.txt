project(${bare_name}-test_cli CXX C)

add_executable(${PROJECT_NAME}
	${CMAKE_SOURCE_DIR}/src/main/damerau_levenshtein.c
	${CMAKE_SOURCE_DIR}/src/main/utf8_ops.c
	test_damerau_levenshtein.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/src
	PRIVATE ${CMAKE_SOURCE_DIR}/lib/pbl/pbl/src/src
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ../gtest/include
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-common
	PRIVATE ${bare_name}-machinery
	PRIVATE gtest
	PRIVATE gtest_main
	PRIVATE pbl
)
