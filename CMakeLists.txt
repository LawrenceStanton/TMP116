cmake_minimum_required(VERSION 3.24)

project(TMP116)

set(LIBRARY ${PROJECT_NAME})

enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${LIBRARY} STATIC
	Src/TMP116.cpp
)

target_include_directories(${LIBRARY} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

add_library(${LIBRARY}::${LIBRARY} ALIAS ${LIBRARY})

if(NOT CMAKE_CROSSCOMPILING)
	OPTION(TMP116_CODE_COVERAGE "Enable gcovr code coverage for TMP116" OFF)
	option(TMP116_AUTOFETCH_GTEST "Automatically fetch googletest if not included by parent." ON)

	if(TMP116_AUTOFETCH_GTEST)
		if(NOT TARGET GTest::gtest OR NOT TARGET GTest::gmock)
			message(STATUS "GTest not found. Fetching...")

			include(FetchContent)
			FetchContent_Declare(
				googletest
				GIT_REPOSITORY https://github.com/google/googletest
				GIT_TAG        origin/main
			)
			set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
			FetchContent_MakeAvailable(googletest)

			enable_testing()
		endif()
	endif()

	target_link_libraries(${LIBRARY} PRIVATE
		GTest::gtest
	)

	target_compile_options(${LIBRARY} PRIVATE
		$<$<BOOL:${TMP116_CODE_COVERAGE}>:--coverage>
	)

	target_link_options(${LIBRARY} PRIVATE
		$<$<BOOL:${TMP116_CODE_COVERAGE}>:--coverage>
	)

	# GTest Test Executable
	set(TEST_EXECUTABLE ${LIBRARY}_Test)
	
	add_executable(${TEST_EXECUTABLE}
		Test/TMP116.test.cpp
		Test/TMP116_Config.test.cpp
	)

	target_compile_options(${TEST_EXECUTABLE} PRIVATE
		$<$<BOOL:${TMP116_CODE_COVERAGE}>:--coverage>
	)

	target_link_libraries(${TEST_EXECUTABLE} PRIVATE
		${LIBRARY}::${LIBRARY}
		GTest::gtest_main
		GTest::gmock
	)

	target_link_options(${TEST_EXECUTABLE} PRIVATE
		$<$<BOOL:${TMP116_CODE_COVERAGE}>:--coverage>
	)

	include(GoogleTest)
	gtest_discover_tests(${TEST_EXECUTABLE})

	if(TMP116_CODE_COVERAGE)
		set(GCOVR_COMMAND gcovr --root ${CMAKE_SOURCE_DIR} --gcov-executable gcov-13 --filter '.*/TMP116/.*' --exclude '.*\.test\..*' ${CMAKE_CURRENT_BINARY_DIR})
		set(SILENT_RUN_COMMAND ./${TEST_EXECUTABLE} > /dev/null)

		add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD
			COMMENT "Running gcovr Code Coverage for TMP116 Tests..."
			COMMAND ${SILENT_RUN_COMMAND} && ${GCOVR_COMMAND} || echo "Code coverage failed. All tests must pass for code coverage to run."
		)
	endif()
endif()
