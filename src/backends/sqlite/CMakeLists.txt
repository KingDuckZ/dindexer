project(${bare_name}-backend-sqlite CXX)

add_library(${PROJECT_NAME} SHARED
	backend_sqlite.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
	PRIVATE SQLiteCpp
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
ln_backend(${PROJECT_NAME})
