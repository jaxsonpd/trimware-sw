# CMake support for Unity test framework
# Harry Mander 2024
# Unity is copyright of Mike Karlesky, Mark VanderVoord, Greg Williams

find_program(RUBY NAMES ruby ruby3.0 ruby2.0 REQUIRED)

set(UNITY_DIR ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)

add_library(unity STATIC unity.c)
target_compile_definitions(unity PUBLIC UNITY_USE_COMMAND_LINE_ARGS=1)
target_include_directories(unity PUBLIC .)

function(add_unity_test TARGET MAIN_TEST_FILE)
    # The following is based on GTest's CMake integration. We use the provided
    # Ruby from Unity to parse the test file and produce a test runner C file
    # with a main function that can run all tests, list the tests, or just run
    # specific tests. Then we add a post-build command on the test runner
    # executable that runs the executable to get a list of test names, and then
    # generate CTest tests from these names.

    set(runner_src ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.runner.c)
    set(ctest_src ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.runner.cmake)

    add_custom_command(
        OUTPUT ${runner_src}
        COMMENT "Generating test runner for ${MAIN_TEST_FILE}"
        COMMAND
            ${RUBY} ${UNITY_DIR}/auto/generate_test_runner.rb
            --cmdline_args=1
            ${MAIN_TEST_FILE} ${runner_src}
        DEPENDS
            ${MAIN_TEST_FILE}
            ${UNITY_DIR}/auto/generate_test_runner.rb
            ${UNITY_DIR}/auto/run_test.erb
            ${UNITY_DIR}/auto/type_sanitizer.rb
            ${UNITY_DIR}/auto/yaml_helper.rb
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    add_executable(${TARGET} ${runner_src} ${MAIN_TEST_FILE} ${ARGN})
    target_link_libraries(${TARGET} PRIVATE unity)
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    set(test_base "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}")
    set(tests_file "${test_base}_tests.cmake")
    set(include_file "${test_base}_include.cmake")
    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        BYPRODUCTS ${tests_file}
        COMMAND
            ${CMAKE_COMMAND}
                -DTEST_RUNNER=$<TARGET_FILE:${TARGET}>
                -DTEST_GROUP=${TARGET}
                -DTEST_FILE=${tests_file}
                -P ${UNITY_DIR}/add_unity_tests.cmake
    )

    # When ctest runs, it will include ${include_file}, which itself includes
    # ${tests_file} that adds the tests.
    file(WRITE "${include_file}"
        "if(EXISTS \"${tests_file}\")\n"
        "  include(\"${tests_file}\")\n"
        "else()\n"
        "  add_test(${TARGET}_NOT_BUILT ${TARGET}_NOT_BUILT)\n"
        "endif()\n"
    )
    set_property(DIRECTORY
      APPEND PROPERTY TEST_INCLUDE_FILES "${include_file}"
    )

endfunction()
