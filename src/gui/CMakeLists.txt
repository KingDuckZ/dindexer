project(${bare_name}-gui CXX)

option(DINDEXER_ENABLE_QRC "Bundle Qt5 resources in the final binary" OFF)

set_switchable(PREFIX DINDEXER INFIX INST NAME QML_PATH
	ONVALUE "share/${bare_name}/qml" CACHE STRING "Path to the directory containing the qml files needed by ${bare_name} gui"
	OFFVALUE "resources/qml" CACHE STRING "Path to the directory containing the qml files needed by ${bare_name} gui"
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Core 5.1 REQUIRED)
find_package(Qt5Qml 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)

add_executable(${PROJECT_NAME}
	main.cpp
	searcher.cpp
	$<$<AND:$<CONFIG:Release>,$<BOOL:${DINDEXER_ENABLE_QRC}>>:${CMAKE_CURRENT_BINARY_DIR}/reslist.qrc>
)

set(qml_files
	qml/mainwin.qml
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
	PRIVATE ${CMAKE_SOURCE_DIR}/lib/glob2regex/include
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${bare_name}-if
	PRIVATE ${bare_name}-machinery
	PRIVATE ${bare_name}-common
	PRIVATE Qt5::Qml
	PRIVATE Qt5::Widgets
	PRIVATE Qt5::Core
	PRIVATE glob2regex
)

string(REPLACE "${bare_name}-" "" ACTION_NAME "${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME}
	PRIVATE ACTION_NAME="${ACTION_NAME}"
	PRIVATE DIN_QRC_ENABLED=$<AND:$<CONFIG:Release>,$<BOOL:${DINDEXER_ENABLE_QRC}>>
)

if (DINDEXER_ENABLE_QRC)
	set(resource_list_for_qrc "")
endif()
get_directory_property(clean_qml_list ADDITIONAL_MAKE_CLEAN_FILES)
foreach(current_qml ${qml_files})
	get_filename_component(qml_basename "${current_qml}" NAME)
	if (DINDEXER_ENABLE_QRC)
		set(resource_list_for_qrc "${resource_list_for_qrc}\t<file alias=\"${current_qml}\">${CMAKE_CURRENT_SOURCE_DIR}/${current_qml}</file>\n")
	else()
		configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${current_qml}" "${CMAKE_BINARY_DIR}/${DINDEXER_CURR_QML_PATH}/${qml_basename}" COPYONLY)
	endif()
	list(APPEND clean_qml_list "${CMAKE_BINARY_DIR}/${DINDEXER_CURR_QML_PATH}/${qml_basename}")
endforeach()
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${clean_qml_list}")
unset(clean_qml_list)
unset(current_qml)
unset(qml_basename)

configure_file(
	"${PROJECT_NAME}Config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.h"
)
if (DINDEXER_ENABLE_QRC)
	configure_file(
		"reslist.qrc.in"
		"${CMAKE_CURRENT_BINARY_DIR}/reslist.qrc"
	)
	unset(resource_list_for_qrc)
endif()

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION ${ACTIONS_PATH_INSTALL}
	ARCHIVE DESTINATION lib/static
)
if (NOT DINDEXER_ENABLE_QRC)
	install(FILES ${qml_files} DESTINATION "${DINDEXER_CURR_QML_PATH}")
endif()
