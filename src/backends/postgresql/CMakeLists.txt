project(${bare_name}-backend-postgresql CXX)

add_library(${PROJECT_NAME} STATIC
	tag.cpp
	delete.cpp
	locate.cpp
	scan.cpp
	dbsource.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${CMAKE_SOURCE_DIR}/include/backends/postgresql
)
#target_include_directories(${PROJECT_NAME} SYSTEM
#)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-pq
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
