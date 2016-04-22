if (NOT bare_name)
	message(FATAL_ERROR "bare_name variable not defined")
endif()

string(TOUPPER "${bare_name}" bare_name_upper)

option(${bare_name_upper}_WITH_BUILD_DATE "Compile the build date into the program. It will cause frequent rebuilds" OFF)
string(TOUPPER ${${bare_name_upper}_WITH_BUILD_DATE} ${bare_name_upper}_WITH_BUILD_DATE PARENT_SCOPE)

#See:
#http://stackoverflow.com/questions/24292898/compile-date-and-time-using-cmake#26214363
function (make_timestamp project_name file_input)
#	if (${bare_name_upper}_WITH_BUILD_DATE)
#		string(TIMESTAMP ${bare_name_upper}_BUILD_DATE "%Y-%m-%d %H:%M:%S" UTC)
#	else()
#		set(${bare_name_upper}_BUILD_DATE "")
#	endif()
	get_filename_component(file_name "${file_input}" NAME)
	string(REGEX REPLACE "\\.in$" "" file_name "${file_name}")

	FILE (WRITE ${CMAKE_CURRENT_BINARY_DIR}/timestamp.cmake
"if (${bare_name_upper}_WITH_BUILD_DATE)
#	string(TIMESTAMP ${bare_name_upper}_BUILD_DATE \"%Y-%m-%d %H:%M:%S\" UTC)
	string(TIMESTAMP ${bare_name_upper}_BUILD_DATE \"%Y-%m-%d\" UTC)
else()
	set(${bare_name_upper}_BUILD_DATE \"\")
endif()
#set(${bare_name_upper}_BUILD_DATE \"\${${bare_name_upper}_BUILD_DATE}\" PARENT_SCOPE)
configure_file(\"${CMAKE_CURRENT_SOURCE_DIR}/${file_input}\" \"${CMAKE_CURRENT_BINARY_DIR}/${file_name}\")\n"
	)

	add_custom_target (
		timestamp
		COMMAND ${CMAKE_COMMAND} "-D${bare_name_upper}_WITH_BUILD_DATE=${${bare_name_upper}_WITH_BUILD_DATE}" -P ${CMAKE_CURRENT_BINARY_DIR}/timestamp.cmake
		ADD_DEPENDENCIES ${CMAKE_CURRENT_BINARY_DIR}/timestamp.cmake
	)

	add_dependencies(${project_name} timestamp)
	#set_source_files_properties("${PROJECT_BINARY_DIR}/${bare_name}Config.h" OBJECT_DEPENDS timestamp)
endfunction()
