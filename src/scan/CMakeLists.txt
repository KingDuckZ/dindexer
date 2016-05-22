project(${bare_name}-scan CXX)

include(WithMediaAutodetect)

add_executable(${PROJECT_NAME}
	main.cpp
	commandline.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-common
	PRIVATE ${bare_name}-machinery
	PRIVATE ${bare_name}-backend-postgresql
)

string(REPLACE "${bare_name}-" "" ACTION_NAME "${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME}
	PRIVATE ACTION_NAME="${ACTION_NAME}"
)

if (DINDEXER_WITH_MEDIA_AUTODETECT)
	target_compile_definitions(${PROJECT_NAME}
		PRIVATE WITH_MEDIA_AUTODETECT
	)
endif()

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION ${ACTIONS_PATH_INSTALL}
	ARCHIVE DESTINATION lib/static
)
