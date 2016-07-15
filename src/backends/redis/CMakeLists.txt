project(${bare_name}-backend-redis CXX)

set(DINDEXER_REDIS_SCRIPTS_PATH "${CMAKE_INSTALL_PREFIX}/${bare_name}/redis" CACHE STRING "Path where Lua scripts for Redis are stored")

find_package(Boost 1.53.0 REQUIRED COMPONENTS regex)

add_library(${PROJECT_NAME} SHARED
	backend_redis.cpp
	tag.cpp
	delete.cpp
	find.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${Boost_INCLUDE_DIRS}
)
target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
	PUBLIC ${CMAKE_SOURCE_DIR}/src/incredis
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
	PUBLIC ${bare_name}-core
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE incredis
)

configure_file(
	redisConfig.h.in
	${CMAKE_CURRENT_BINARY_DIR}/redisConfig.h
)
set(LUA_SCRIPTS
	tag_if_in_set.lua
	dele_tag_if_in_set.lua
	dele_hash.lua
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)

set(lua_script_list "")
foreach (lua_script ${LUA_SCRIPTS})
	get_filename_component(lua_script_basename "${lua_script}" NAME)
	configure_file("${lua_script}" "${CMAKE_CURRENT_BINARY_DIR}/lua/${lua_script_basename}" COPYONLY)
	list(APPEND lua_script_list "${CMAKE_CURRENT_BINARY_DIR}/lua/${lua_script_basename}")
endforeach()
unset(lua_script)
unset(lua_script_basename)
install(FILES ${lua_script_list} DESTINATION "${DINDEXER_REDIS_SCRIPTS_PATH}")
unset(lua_script_list)

ln_backend(${PROJECT_NAME})
