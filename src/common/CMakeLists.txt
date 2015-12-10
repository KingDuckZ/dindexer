project(${bare_name}-common CXX C)

add_library(${PROJECT_NAME}
	commandline.cpp
	mediatypes.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${DINDEXER_PUB_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
)
