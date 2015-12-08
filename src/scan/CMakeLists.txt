project(${bare_name}-scan CXX C)

find_package(blkid REQUIRED)

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
	discinfo.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${BLKID_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${BLKID_LIBRARIES}
)
