project(${bare_name}-backend-postgresql CXX)

add_library(${PROJECT_NAME} STATIC
	tag.cpp
)

#target_include_directories(${PROJECT_NAME}
#)
#target_include_directories(${PROJECT_NAME} SYSTEM
#)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE pq
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
