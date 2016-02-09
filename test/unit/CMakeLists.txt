project(${bare_name}-test CXX)

add_executable(${PROJECT_NAME}
	test_diriterator.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ../gtest/include
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE gtest
	PRIVATE gtest_main
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-common
	PRIVATE ${bare_name}-machinery
)
