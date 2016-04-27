project(${bare_name}-navigate CXX)

add_executable(${PROJECT_NAME}
	main.cpp
	commandline.cpp
	commandprocessor.cpp
	genericpath.cpp
	dbsource.cpp
	linereader.cpp
	listdircontent.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-common
)

target_compile_features(${PROJECT_NAME}
	PUBLIC cxx_alignof
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
