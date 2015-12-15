project(${bare_name}-scan CXX C)

if (DINDEXER_WITH_MEDIA_AUTODETECT)
	find_package(blkid)
	if (NOT BLKID_FOUND)
		message(STATUS "libblkid not found, media autodetection will be disabled")
		set(DINDEXER_WITH_MEDIA_AUTODETECT OFF)
	endif()
endif()

find_package(Magic REQUIRED)

add_executable(${PROJECT_NAME}
	main.cpp
	filesearcher.cpp
	pathname.cpp
	indexer.cpp
	tiger.c
	tiger.cpp
	dbbackend.cpp
	commandline.cpp
	discinfo.cpp
	mediatype.cpp
	mimetype.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_include_directories(${PROJECT_NAME}
	PRIVATE ${MAGIC_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-common
	PRIVATE ${MAGIC_LIBRARIES}
)

string(REPLACE "${bare_name}-" "" ACTION_NAME "${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME}
	PRIVATE ACTION_NAME="${ACTION_NAME}"
)

if (DINDEXER_WITH_MEDIA_AUTODETECT)
	target_include_directories(${PROJECT_NAME} SYSTEM
		PRIVATE ${BLKID_INCLUDE_DIRS}
	)
	target_link_libraries(${PROJECT_NAME}
		PRIVATE ${BLKID_LIBRARIES}
	)
	target_compile_definitions(${PROJECT_NAME}
		PRIVATE WITH_MEDIA_AUTODETECT
	)
endif()
