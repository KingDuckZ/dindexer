project(${bare_name}-common CXX C)

add_library(${PROJECT_NAME}
	commandline.cpp
	mediatypes.cpp
	settings.cpp
	validationerror.cpp
	common_info.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${DINDEXER_PUB_INCLUDE_DIR}
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${YAMLCPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${YAMLCPP_LIBRARY}
)

#install(TARGETS ${PROJECT_NAME}
#	LIBRARY DESTINATION lib
#	RUNTIME DESTINATION bin
#	ARCHIVE DESTINATION lib/static
#)
