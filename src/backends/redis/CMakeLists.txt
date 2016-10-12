project(${bare_name}-backend-redis CXX)

set_switchable(PREFIX DINDEXER INFIX INST NAME REDIS_SCRIPTS_PATH
	ONVALUE "share/${bare_name}/redis" CACHE STRING "Path where Lua scripts for Redis are stored"
	OFFVALUE "resources/redis" CACHE STRING "Path where Lua scripts for Redis are stored"
)

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
	lua/tag_if_in_set.lua
	lua/dele_tag_if_in_set.lua
	lua/dele_hash.lua
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)

get_directory_property(lua_script_list ADDITIONAL_MAKE_CLEAN_FILES)
foreach (current_lua ${LUA_SCRIPTS})
	get_filename_component(lua_script_basename "${current_lua}" NAME)
	configure_file("${current_lua}" "${CMAKE_BINARY_DIR}/${DINDEXER_CURR_REDIS_SCRIPTS_PATH}/${lua_script_basename}" COPYONLY)
	list(APPEND lua_script_list "${CMAKE_BINARY_DIR}/${DINDEXER_CURR_REDIS_SCRIPTS_PATH}/${lua_script_basename}")
endforeach()
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${lua_script_list}")
unset(lua_script_list)
unset(current_lua)
unset(lua_script_basename)

install(FILES ${LUA_SCRIPTS} DESTINATION "${DINDEXER_CURR_REDIS_SCRIPTS_PATH}")

ln_backend(${PROJECT_NAME})
