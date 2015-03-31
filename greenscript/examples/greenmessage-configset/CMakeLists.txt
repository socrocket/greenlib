SET(EXECUTABLE "greenmessage-configset")

#
# This is needed when 'make check' is executed out of tree.
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/receiver.py
               ${CMAKE_CURRENT_BINARY_DIR}/receiver.py COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sender.py
               ${CMAKE_CURRENT_BINARY_DIR}/sender.py COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/system.py
               ${CMAKE_CURRENT_BINARY_DIR}/system.py COPYONLY)

add_test(NAME COMPILATION:_${EXECUTABLE}
         COMMAND  sh -c
"export PYTHONPATH=$ENV{PYTHONPATH} && python system.py >executable_output.txt")
add_test(NAME DIFF:_${EXECUTABLE}
         COMMAND ${CMAKE_COMMAND} -E compare_files executable_output.txt
                                ${CMAKE_CURRENT_SOURCE_DIR}/expected_output.txt)
