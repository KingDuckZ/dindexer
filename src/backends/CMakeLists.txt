project(${bare_name}-backend CXX)

set(BACKEND_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(DINDEXER_ENABLED_BACKENDS "postgresql" CACHE STRING "Comma separated list of enabled backends. See directories in backends source directory for a list of known backends.")

function(ln_backend backend_name)
	add_custom_command(TARGET "${backend_name}" POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:${backend_name}> ${BACKEND_BINARY_DIR}/$<TARGET_FILE_NAME:${backend_name}>
		DEPENDS ${BACKEND_BINARY_DIR}/$<TARGET_FILE_NAME:${backend_name}>
		COMMENT "Creating symlink to ${backend_name}"
	)
endfunction()

add_library(${PROJECT_NAME} STATIC
	backend_loader.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${DINDEXER_PUB_INCLUDE_DIR}
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${YAMLCPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	INTERFACE ${YAMLCPP_LIBRARY}
	INTERFACE dl
	INTERFACE ${bare_name}-machinery
)

string(REPLACE "," ";" backend_list "${DINDEXER_ENABLED_BACKENDS}")
foreach(backend ${backend_list})
	add_subdirectory(${backend})
	add_dependencies(${PROJECT_NAME} ${bare_name}-backend-${backend})
endforeach()
unset(backend)
