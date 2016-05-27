project(${bare_name}-pq CXX)

set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")

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

target_compile_features(${PROJECT_NAME}
	PUBLIC cxx_constexpr
	PUBLIC cxx_alignof
)

#install(TARGETS ${PROJECT_NAME}
#	LIBRARY DESTINATION lib
#	RUNTIME DESTINATION bin
#	ARCHIVE DESTINATION lib/static
#)
