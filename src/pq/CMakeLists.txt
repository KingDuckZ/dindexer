project(dindexer-pq CXX)

add_library(${PROJECT_NAME} STATIC
	connection.cpp
	databaseexception.cpp
	resultset.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE dindexer
)
