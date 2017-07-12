SET(EXECUTABLE "greensignalsocket_corner_cases")

add_executable(${EXECUTABLE} example.cpp)
target_link_libraries(${EXECUTABLE} ${SystemC_LIBRARIES} pthread)
add_dependencies(compile_test ${EXECUTABLE})

add_test(NAME COMPILATION_TEST0:_${EXECUTABLE}
         COMMAND  sh -c "./${EXECUTABLE} 0 > executable_output.txt")
add_test(NAME TEMPLATE_TEST0:_${EXECUTABLE}
         COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake)
add_test_compare(DIFF_TEST_0:${EXECUTABLE}
	executable_output.txt ${CMAKE_CURRENT_SOURCE_DIR}/expected_output_0)
add_test(NAME COMPILATION_TEST1:_${EXECUTABLE}
         COMMAND  sh -c "./${EXECUTABLE} 1 > executable_output.txt")
add_test(NAME TEMPLATE_TEST1:_${EXECUTABLE}
         COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake)
add_test_compare(DIFF_TEST_1:${EXECUTABLE}
	executable_output.txt ${CMAKE_CURRENT_SOURCE_DIR}/expected_output_1)
add_test(NAME COMPILATION_TEST2:_${EXECUTABLE}
         COMMAND  sh -c "./${EXECUTABLE} 2 > executable_output.txt")
add_test(NAME TEMPLATE_TEST2:_${EXECUTABLE}
         COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake)
add_test_compare(DIFF_TEST_2:${EXECUTABLE}
	executable_output.txt ${CMAKE_CURRENT_SOURCE_DIR}/expected_output_2)
add_test(NAME COMPILATION_TEST3:_${EXECUTABLE}
         COMMAND  sh -c "./${EXECUTABLE} 3 > executable_output.txt")
add_test(NAME TEMPLATE_TEST3:_${EXECUTABLE}
         COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake)
add_test_compare(DIFF_TEST_3:${EXECUTABLE}
	executable_output.txt ${CMAKE_CURRENT_SOURCE_DIR}/expected_output_3)

set_tests_properties(COMPILATION_TEST0:_${EXECUTABLE}
                     COMPILATION_TEST1:_${EXECUTABLE}
                     COMPILATION_TEST2:_${EXECUTABLE}
                     COMPILATION_TEST3:_${EXECUTABLE} PROPERTIES WILL_FAIL TRUE)
