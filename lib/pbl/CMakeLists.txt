cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(pbl C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELEASE} -Wall -Wno-sign-compare")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wno-sign-compare -O3")

option(PBL_WITH_TESTS "Enable building test programs" ON)

if (PBL_WITH_TESTS)
	include(CTest)
endif()

add_library(pbl STATIC
	pbl/src/src/pblStringBuilder.c
	pbl/src/src/pblPriorityQueue.c
	pbl/src/src/pblHeap.c
	pbl/src/src/pblMap.c
	pbl/src/src/pblSet.c
	pbl/src/src/pblList.c
	pbl/src/src/pblCollection.c
	pbl/src/src/pblIterator.c
	pbl/src/src/pblisam.c
	pbl/src/src/pblkf.c
	pbl/src/src/pblhash.c
	pbl/src/src/pbl.c
)
target_include_directories(pbl
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/src
)
target_compile_definitions(pbl
	PUBLIC PBLTEST
)

if (BUILD_TESTING AND PBL_WITH_TESTS)
	set(pbl_tests
		httst
		#kftst
		iftst
		kfblockprint
		ListTest
		SetTest
		Perform
		MapTest
		PriorityQueueTest
	)

	foreach (test_name ${pbl_tests})
		add_executable(pbl${test_name} pbl/src/src/pbl${test_name}.c)
		target_link_libraries(pbl${test_name} pbl)
	endforeach()
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/pbl/src/test" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
	#Instead of using
	#file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/isamtest")
	#use configure_file to put a placeholder. This way, if the directory
	#isamtest is removed by a 'make clean' command, it will trigger a reconf
	#as cmake will try to re-create the placeholder file.
	configure_file(CMakeLists.txt "${CMAKE_CURRENT_BINARY_DIR}/isamtest/placeholder.file" COPYONLY)
	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "pbliftst.log;pblListTest.log;pblSetTest.log;isamtest")
	file(WRITE
		"${CMAKE_CURRENT_BINARY_DIR}/run_pbl_test.cmake"
		"file(GLOB dele_list isamtest/0*)\nlist(LENGTH dele_list dele_list_len)\nif (dele_list_len GREATER 0)\n\texecute_process(COMMAND \${CMAKE_COMMAND} -E remove -f \${dele_list})\nendif()\nexecute_process(COMMAND ./\${PBL_TEST} \${PBL_ARG})\n"
	)

	#I think tests need to find that their own files don't exist whene they are
	#started. Thish means that the correct way to run these tests is the
	#sequence of commannds rm -f isamtest/0* ; ./test param.tst
	#However, add_test doesn't support running multiple commands (see
	#https://cmake.org/pipermail/cmake/2006-April/008976.html - reply to that is
	#"just add a wrapper program that removes the file and then invokes
	#exampleprogram"). The solution is then to have a wrapper script called
	#run_pbl_test.cmake, which is generated above by the file(WRITE) command.
	#The script does just that, rm -f and then invokes the PBL_TEST parameter.
	add_test(NAME pblhttst_1 COMMAND pblhttst)
	add_test(NAME pbliftst_1 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pbliftst -DPBL_ARG=test/isam0001.tst -P run_pbl_test.cmake)
	add_test(NAME pbliftst_2 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pbliftst -DPBL_ARG=test/isam0002.tst -P run_pbl_test.cmake)
	add_test(NAME pbliftst_3 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pbliftst -DPBL_ARG=test/isam0004.tst -P run_pbl_test.cmake)
	add_test(NAME pbliftst_4 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pbliftst -DPBL_ARG=test/isam0005.tst -P run_pbl_test.cmake)
	add_test(NAME pbliftst_5 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pbliftst -DPBL_ARG=test/isam0006.tst -P run_pbl_test.cmake)
	add_test(NAME pblListTest_1 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pblListTest -DPBL_ARG=test/arraylist0001.tst -P run_pbl_test.cmake)
	add_test(NAME pblListTest_2 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pblListTest -DPBL_ARG=test/linkedlist0001.tst -P run_pbl_test.cmake)
	add_test(NAME pblSetTest_1 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pblSetTest -DPBL_ARG=test/treeset0001.tst -P run_pbl_test.cmake)
	add_test(NAME pblSetTest_2 COMMAND ${CMAKE_COMMAND} -DPBL_TEST=pblSetTest -DPBL_ARG=test/hashset0001.tst -P run_pbl_test.cmake)
	add_test(NAME pblPerform_1 COMMAND pblPerform)
	add_test(NAME pblMapTest_1 COMMAND pblMapTest)
	add_test(NAME pblPriorityQueueTest_1 COMMAND pblPriorityQueueTest)
endif()
