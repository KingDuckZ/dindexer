project(${bare_name}-pq CXX)

add_library(${PROJECT_NAME} STATIC
	connection.cpp
	databaseexception.cpp
	resultset.cpp
	pq_type_helpers.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${libpqtypes_INCLUDE_DIRS}
	PRIVATE ${PostgreSQL_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE pqtypes
	PRIVATE ${PostgreSQL_LIBRARIES}
)

target_compile_features(${PROJECT_NAME}
	PUBLIC cxx_constexpr
	PUBLIC cxx_alignof
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
