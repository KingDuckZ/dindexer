project(${bare_name}-test CXX)

add_executable(${PROJECT_NAME}
	test_diriterator.cpp
	test_guess_content_type.cpp
	test_glob2regex.cpp
	test_tiger_string_conv.cpp
	test_lexical_cast.cpp
	test_make_filerecord_tree.cpp
	test_pathname.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ../gtest/include
	PRIVATE ../../src/machinery
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-common
	PRIVATE ${bare_name}-machinery
	PRIVATE gtest
	PRIVATE gtest_main
	PRIVATE glob2regex
)

add_test(
	NAME LibraryTests
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${PROJECT_NAME}
)
