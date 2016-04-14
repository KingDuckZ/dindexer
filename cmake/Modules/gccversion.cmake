function (set_compiler_ver spliced_version_var)
	set(whole_version "${${spliced_version_var}}")
	set(var_prefix "${spliced_version_var}")
	string(REGEX MATCHALL "[0-9]+" version_nums "${whole_version}")
	list(GET version_nums 0 compiler_MAJOR)
	list(GET version_nums 1 compiler_MINOR)
	list(GET version_nums 2 compiler_PATCH)
	set(${var_prefix}_MAJOR "${compiler_MAJOR}" PARENT_SCOPE)
	set(${var_prefix}_MINOR "${compiler_MINOR}" PARENT_SCOPE)
	set(${var_prefix}_PATCH "${compiler_PATCH}" PARENT_SCOPE)
	unset(compiler_MAJOR)
	unset(compiler_MINOR)
	unset(compiler_PATCH)
	unset(version_nums)
	unset(whole_version)
	unset(var_prefix)
endfunction()

if (NOT CMAKE_CXX_COMPILER_VERSION)
	if (CMAKE_COMPILER_IS_GNUCXX)
		exec_program(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE version_string)
		string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CMAKE_CXX_COMPILER_VERSION "${version_string}")
		unset(version_string)
	else()
		set(CMAKE_CXX_COMPILER_VERSION "0.0.0")
	endif()
endif()

if (NOT CMAKE_C_COMPILER_VERSION)
	if (CMAKE_COMPILER_IS_GNUCC)
		exec_program(${CMAKE_C_COMPILER} ARGS --version OUTPUT_VARIABLE version_string)
		string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CMAKE_C_COMPILER_VERSION "${version_string}")
		unset(version_string)
	else()
		set(CMAKE_C_COMPILER_VERSION "0.0.0")
	endif()
endif()

set_compiler_ver(CMAKE_CXX_COMPILER_VERSION)
set_compiler_ver(CMAKE_C_COMPILER_VERSION)
