#Find libblkid
#
#	BLKID_INCLUDE_DIRS	-	where to find blkid/blkid.h
#	BLKID_LIBRARIES		-	list of libraries when using libblkid
#	BLKID_FOUND			-	true if libblkid found
#	BLKID_VERSION_STRING-	the version of the libblkid found
#


find_path(BLKID_INCLUDE_DIR NAMES blkid/blkid.h)
mark_as_advanced(BLKID_INCLUDE_DIR)

find_library(BLKID_LIBRARY NAMES
	blkid
)
mark_as_advanced(BLKID_LIBRARY)

if(BLKID_INCLUDE_DIR)
	if(EXISTS "${BLKID_INCLUDE_DIR}/blkid/blkid.h")
		file(STRINGS "${BLKID_INCLUDE_DIR}/blkid/blkid.h" blkid_version_str REGEX "^#define[\t ]+BLKID_VERSION[\t ]+\".*\"")
		string(REGEX REPLACE "^#define[\t ]+BLKID_VERSION[\t ]+\"([^\"]*)\".*" "\\1" BLKID_VERSION_STRING "${blkid_version_str}")
		unset(blkid_version_str)
	endif()
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BLKID
	REQUIRED_VARS BLKID_LIBRARY BLKID_INCLUDE_DIR
	VERSION_VAR BLKID_VERSION_STRING
)

if(BLKID_FOUND)
	set(BLKID_LIBRARIES ${BLKID_LIBRARY})
	set(BLKID_INCLUDE_DIRS ${BLKID_INCLUDE_DIR})
endif()
