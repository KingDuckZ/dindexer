project(${bare_name}-machinery CXX C)

include(WithMediaAutodetect)
find_package(Magic REQUIRED)

add_library(${PROJECT_NAME} SHARED
	indexer.cpp
	pathname.cpp
	tiger.c
	tiger.cpp
	mimetype.cpp
	filesearcher.cpp
	discinfo.cpp
	mediatype.cpp
)

#target_include_directories(${PROJECT_NAME}
#)
target_include_directories(${PROJECT_NAME}
	PRIVATE ${MAGIC_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${MAGIC_LIBRARIES}
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
