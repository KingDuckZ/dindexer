project(${bare_name}-locate CXX)

add_executable(${PROJECT_NAME}
	main.cpp
	commandline.cpp
	postgre_locate.cpp
	hash.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
	PRIVATE ${CMAKE_SOURCE_DIR}/lib/glob2regex/include
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-common
	PRIVATE ${bare_name}-machinery
	PRIVATE glob2regex
)

string(REPLACE "${bare_name}-" "" ACTION_NAME "${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME}
	PRIVATE ACTION_NAME="${ACTION_NAME}"
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION ${ACTIONS_PATH_INSTALL}
	ARCHIVE DESTINATION lib/static
)
