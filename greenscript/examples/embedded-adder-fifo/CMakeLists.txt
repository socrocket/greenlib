SET(EXECUTABLE "embedded-adder-fifo")

ADD_EXECUTABLE(${EXECUTABLE}
	system.cpp
)
target_link_libraries(${EXECUTABLE} ${SystemC_LIBRARIES}
                                    pthread
                                    greenscript
                                    ${PYTHON_LIBRARIES}
                                    ${LUA_LIBRARIES})

#
# This is needed when 'make check' is executed out of tree.
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/adder.py
               ${CMAKE_CURRENT_BINARY_DIR}/adder.py COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.lua
               ${CMAKE_CURRENT_BINARY_DIR}/config.lua COPYONLY)

add_test(NAME COMPILATION:_${EXECUTABLE}
         COMMAND sh -c
"export PYTHONPATH=$ENV{PYTHONPATH} && ./${EXECUTABLE} > executable_output.txt")
add_test(NAME DIFF:_${EXECUTABLE}
         COMMAND ${CMAKE_COMMAND} -E compare_files executable_output.txt
                                ${CMAKE_CURRENT_SOURCE_DIR}/expected_output.txt)

add_dependencies(compile_test ${EXECUTABLE})
