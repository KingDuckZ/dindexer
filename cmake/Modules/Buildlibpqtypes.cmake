include(ExternalProject)

function (import_libpqtypes_project PostgreSQL_INCLUDE_DIRS)
	#find_path(libpqtypes_base_path NAMES src/libpqtypes.h PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib NO_DEFAULT_PATH)
	file(GLOB_RECURSE libpqtypes_base_path "lib/**libpqtypes.h")

	string(REPLACE ";" " -I" libpqtypes_inc_dirs "-I${PostgreSQL_INCLUDE_DIRS}")
	string(REPLACE "src/libpqtypes.h" "" libpqtypes_base_path "${libpqtypes_base_path}")
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" libpqtypes_prefix "${libpqtypes_base_path}")
	string(REGEX REPLACE "/$" "" libpqtypes_prefix "${libpqtypes_prefix}")

	ExternalProject_Add(ext_project_pqtypes
		DOWNLOAD_COMMAND ""
		SOURCE_DIR ${libpqtypes_base_path}
		PREFIX ${libpqtypes_prefix}
		CONFIGURE_COMMAND ${libpqtypes_base_path}/configure --prefix ${libpqtypes_prefix} "CPPFLAGS=${libpqtypes_inc_dirs}" --quiet
		BUILD_COMMAND ${MAKE}
		BUILD_IN_SOURCE 0
	)
	add_library(pqtypes SHARED IMPORTED)
	add_dependencies(pqtypes ext_project_pqtypes)
	set_target_properties(pqtypes PROPERTIES
		IMPORTED_LOCATION ${libpqtypes_prefix}/lib/libpqtypes.so
	)

	if(EXISTS "${libpqtypes_base_path}/configure.ac")
		file(STRINGS "${libpqtypes_base_path}/configure.ac" libpqtypes_version_str REGEX "^AC_INIT[\t ]*\\([\t ]*libpqtypes[\t ]*,[\t ]*[0-9].*$")
		string(REGEX REPLACE "^AC_INIT[\t ]*\\([\t ]*libpqtypes[\t ]*,[\t ]*([^,]+)[\t ]*,.*$" "\\1" libpqtypes_version "${libpqtypes_version_str}")
		unset(libpqtypes_version_str)
		message(STATUS "libpqtypes v${libpqtypes_version} found")
	endif()

	set(libpqtypes_INCLUDE_DIRS "${libpqtypes_prefix}/include" PARENT_SCOPE)

	unset(libpqtypes_prefix)
	unset(libpqtypes_inc_dirs)
endfunction()
