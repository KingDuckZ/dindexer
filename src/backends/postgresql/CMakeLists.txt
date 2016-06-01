project(${bare_name}-backend-postgresql CXX)

add_library(${PROJECT_NAME} SHARED
	tag.cpp
	delete.cpp
	locate.cpp
	scan.cpp
	#dbsource.cpp
	backend_postgresql.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
	PRIVATE ${bare_name}-pq
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
