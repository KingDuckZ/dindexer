project(${bare_name}-core CXX)

add_library(${PROJECT_NAME} SHARED
	searchpaths.cpp
	split_tags.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${DINDEXER_PUB_INCLUDE_DIR}/${bare_name}-core
)

target_compile_features(${PROJECT_NAME}
	INTERFACE cxx_nullptr
	INTERFACE cxx_range_for
	INTERFACE cxx_lambdas
	INTERFACE cxx_decltype_auto
	INTERFACE cxx_defaulted_functions
	INTERFACE cxx_deleted_functions
	INTERFACE cxx_auto_type
	INTERFACE cxx_decltype_incomplete_return_types
	INTERFACE cxx_defaulted_move_initializers
	INTERFACE cxx_noexcept
	INTERFACE cxx_rvalue_references
	INTERFACE cxx_generalized_initializers
	INTERFACE cxx_variadic_templates
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
