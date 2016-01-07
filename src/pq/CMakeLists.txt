project(${bare_name}-pq CXX)

add_library(${PROJECT_NAME} STATIC
	connection.cpp
	databaseexception.cpp
	resultset.cpp
	pq_type_helpers.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${libpqtypes_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE pqtypes
)
