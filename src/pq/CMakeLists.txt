project(${bare_name}-pq CXX)

add_library(${PROJECT_NAME} STATIC
	connection.cpp
	databaseexception.cpp
	resultset.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
)