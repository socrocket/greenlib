SET(EXECUTABLE "greenthreads_async_wait")

add_executable(${EXECUTABLE} main.cpp)
if(MINGW)
    target_link_libraries(${EXECUTABLE} ${SystemC_LIBRARIES} pthread greenthreads)
else()
    target_link_libraries(${EXECUTABLE} ${SystemC_LIBRARIES} pthread rt greenthreads)
endif()

add_dependencies(compile_test ${EXECUTABLE})

add_test(NAME COMPILATION:_${EXECUTABLE}
         COMMAND  sh -c "./${EXECUTABLE} > executable_output.txt")
add_test(NAME DIFF:_${EXECUTABLE}
         COMMAND ${CMAKE_COMMAND} -E compare_files executable_output.txt
                                ${CMAKE_CURRENT_SOURCE_DIR}/expected_output.txt)

