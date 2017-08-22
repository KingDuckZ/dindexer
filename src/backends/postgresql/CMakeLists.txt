project(${bare_name}-backend-postgresql CXX C)

find_package(ZLIB)

include(binary_resource)

if (ZLIB_FOUND)
	set(gzip GZIP)
else()
	set(gzip "")
endif()
make_binary_resource(${gzip}
	INPUT ${CMAKE_BINARY_DIR}/dindexer.sql
	ARRAY_NAME create_tables_query
)

add_library(${PROJECT_NAME} SHARED
	tag.cpp
	delete.cpp
	locate.cpp
	scan.cpp
	navigate.cpp
	backend_postgresql.cpp
	${CMAKE_CURRENT_BINARY_DIR}/create_tables_query.c
	create_tables.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
	PRIVATE ${bare_name}-pq
)
if (ZLIB_FOUND)
	target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
endif()

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)

configure_file(
	backend_postgresql_config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/backend_postgresql_config.h
)

ln_backend(${PROJECT_NAME})
