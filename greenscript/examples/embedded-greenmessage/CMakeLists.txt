SET(EXECUTABLE "embedded-greenmessage")

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
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bidir.py
               ${CMAKE_CURRENT_BINARY_DIR}/bidir.py COPYONLY)

add_test(NAME COMPILATION:_${EXECUTABLE}
         COMMAND sh -c "export PYTHONPATH=$ENV{PYTHONPATH} && ./${EXECUTABLE}")

add_dependencies(compile_test ${EXECUTABLE})
