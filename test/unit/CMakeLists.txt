project(${bare_name}-test CXX)

add_executable(${PROJECT_NAME}
	test_diriterator.cpp
	test_guess_content_type.cpp
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
)
