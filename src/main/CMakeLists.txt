project(${bare_name} C)

include(WithMediaAutodetect)

add_executable(${PROJECT_NAME}
	main.c
	findactions.c
	builtin_feats.c
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
)

if (DINDEXER_WITH_MEDIA_AUTODETECT)
	target_compile_definitions(${PROJECT_NAME}
		PRIVATE WITH_MEDIA_AUTODETECT
	)
endif()
