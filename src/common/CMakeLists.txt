project(${bare_name}-common CXX)

find_package(Readline 6.3 REQUIRED)

add_library(${PROJECT_NAME}
	commandline.cpp
	settings.cpp
	validationerror.cpp
	common_info.cpp
	readline_wrapper.cpp
	split_tags.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${DINDEXER_PUB_INCLUDE_DIR}
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${YAMLCPP_INCLUDE_DIR}
	PRIVATE ${Readline_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${YAMLCPP_LIBRARY}
	PRIVATE ${Readline_LIBRARY}
	${bare_name}-backend
	${bare_name}-core
)

#install(TARGETS ${PROJECT_NAME}
#	LIBRARY DESTINATION lib
#	RUNTIME DESTINATION bin
#	ARCHIVE DESTINATION lib/static
#)

#Allow linking with .so
#see https://cmake.org/pipermail/cmake/2007-May/014350.html
#and http://stackoverflow.com/questions/6093547/what-do-r-x86-64-32s-and-r-x86-64-64-relocation-mean/6093910#6093910
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
