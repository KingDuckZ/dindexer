project(${bare_name}-backend-sqlite CXX)

if (SQLITECPP_INTERNAL_SQLITE)
	set(sqlite3_lib "sqlite3")
else()
	find_package(sqlite3 REQUIRED)
	set(sqlite3_lib "SQLite3::SQLite3")
endif()

add_library(${PROJECT_NAME} SHARED
	backend_sqlite.cpp
	db_functions.cpp
	time_t_to_timestamp.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
	PRIVATE SQLiteCpp
	PRIVATE ${sqlite3_lib}
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
ln_backend(${PROJECT_NAME})
