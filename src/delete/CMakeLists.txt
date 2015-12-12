project(${bare_name}-delete CXX C)

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
)

string(REPLACE "${bare_name}-" "" ACTION_NAME "${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME}
	PRIVATE ACTION_NAME="${ACTION_NAME}"
)

