project(${bare_name} C)

include(WithMediaAutodetect)

add_executable(${PROJECT_NAME}
	main.c
	findactions.c
	builtin_feats.c
	damerau_levenshtein.c
	utf8_ops.c
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/include
	PRIVATE ${CMAKE_SOURCE_DIR}/lib/pbl/pbl/src/src
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
	PRIVATE pbl
)

if (DINDEXER_WITH_MEDIA_AUTODETECT)
	target_compile_definitions(${PROJECT_NAME}
		PRIVATE WITH_MEDIA_AUTODETECT
	)
endif()

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
