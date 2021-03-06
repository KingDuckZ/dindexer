project(${bare_name}-machinery CXX C)

include(WithMediaAutodetect)
find_package(Magic REQUIRED)

option(DINDEXER_WITH_NICE_MEDIA_TYPES "Enable this if C++14 is available in order to get some nicer media type names, like CD-Rom instead of CDRom" OFF)

add_library(${PROJECT_NAME} SHARED
	pathname.cpp
	tiger.c
	tiger.cpp
	mimetype.cpp
	filesearcher.cpp
	discinfo.cpp
	mediatype.cpp
	mediatypes.cpp
	machinery_info.cpp
	guess_content_type.cpp
	set_listing.cpp
	globbing.cpp
	scantask/dirtree.cpp
	scantask/mediatype.cpp
	scantask/hashing.cpp
	scantask/contenttype.cpp
	scantask/mime.cpp
	scantask/setbasic.cpp
	scantask/singlefile.cpp
	make_filerecord_tree.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${MAGIC_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${MAGIC_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_SOURCE_DIR}/lib/better-enums
)

target_compile_features(${PROJECT_NAME}
	PUBLIC cxx_constexpr
	PUBLIC cxx_nullptr
	PUBLIC cxx_override
)
if (DINDEXER_WITH_NICE_MEDIA_TYPES)
	target_compile_features(${PROJECT_NAME} PRIVATE cxx_relaxed_constexpr)
endif()

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

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)

configure_file(machineryConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/machineryConfig.h)
