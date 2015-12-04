project(${bare_name} C)

add_executable(${PROJECT_NAME}
	main.c
	findactions.c
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
)
