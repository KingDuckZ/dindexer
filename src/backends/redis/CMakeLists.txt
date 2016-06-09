project(${bare_name}-backend-redis CXX)

find_package(hiredis 0.11.0 REQUIRED)

add_library(${PROJECT_NAME} SHARED
	backend_redis.cpp
	command.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${Boost_INCLUDE_DIRS}
	PRIVATE ${HIREDIS_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
	PRIVATE ${bare_name}-pq
	PRIVATE ${HIREDIS_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
ln_backend(${PROJECT_NAME})
