project(${bare_name}-backend-redis CXX)

set(DINDEXER_REDIS_SCRIPTS_PATH "${CMAKE_INSTALL_PREFIX}/${bare_name}/redis" CACHE STRING "Path where Lua scripts for Redis are stored")

find_package(hiredis 0.11.0 REQUIRED)
find_package(CryptoPP 5.6)
find_package(libev 4.0 REQUIRED)

add_library(${PROJECT_NAME} SHARED
	backend_redis.cpp
	command.cpp
	scan_iterator.cpp
	reply.cpp
	batch.cpp
	script.cpp
	script_manager.cpp
	async_connection.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${Boost_INCLUDE_DIRS}
	PRIVATE ${HIREDIS_INCLUDE_DIRS}
	PRIVATE ${CMAKE_SOURCE_DIR}/lib/better-enums
	PRIVATE ${LIBEV_INCLUDE_DIRS}
)
target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-inc
	PUBLIC ${bare_name}-core
	PRIVATE ${HIREDIS_LIBRARIES}
	PRIVATE ${LIBEV_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE EV_COMPAT3=0
)

if (CryptoPP_FOUND)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${CryptoPP_LIBRARIES})
	target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CryptoPP_INCLUDE_DIRS})
	set (has_cryptopp_lib ON)
else()
	set (has_cryptopp_lib OFF)
endif()

configure_file(
	redisConfig.h.in
	${CMAKE_CURRENT_BINARY_DIR}/redisConfig.h
)
set(LUA_SCRIPTS
	"${CMAKE_SOURCE_DIR}/lib/ohm-scripts/save.lua"
	"${CMAKE_SOURCE_DIR}/lib/ohm-scripts/delete.lua"
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
