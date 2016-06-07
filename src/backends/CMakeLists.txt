project(${bare_name}-backend CXX)

add_library(${PROJECT_NAME} STATIC
	backend_loader.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${DINDEXER_PUB_INCLUDE_DIR}
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${YAMLCPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	INTERFACE ${YAMLCPP_LIBRARY}
	INTERFACE dl
	INTERFACE ${bare_name}-machinery
)

#install(TARGETS ${PROJECT_NAME}
#	LIBRARY DESTINATION lib
#	RUNTIME DESTINATION bin
#	ARCHIVE DESTINATION lib/static
#)

add_subdirectory(postgresql)
add_subdirectory(redis)

add_dependencies(${PROJECT_NAME} ${bare_name}-backend-postgresql)
add_dependencies(${PROJECT_NAME} ${bare_name}-backend-redis)
