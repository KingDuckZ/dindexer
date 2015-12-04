project(${bare_name}-scan CXX C)

add_executable(${PROJECT_NAME}
	main.cpp
	filesearcher.cpp
	pathname.cpp
	indexer.cpp
	tiger.c
	tiger.cpp
	dbbackend.cpp
	settings.cpp
	commandline.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
)
