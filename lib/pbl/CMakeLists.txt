cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(pbl C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELEASE} -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")

option(PBL_WITH_TESTS "Enable building test programs" ON)

#add_library(${PROJECT_NAME} INTERFACE)
#target_include_directories(${PROJECT_NAME}
#	INTERFACE .
#)

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

if (PBL_WITH_TESTS)
	enable_testing()

	set(pbl_tests
		httst
		kftst
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
		add_test(${test_name} pbl${test_name})
	endforeach()
endif()
