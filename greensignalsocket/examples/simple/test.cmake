file(READ "executable_output.txt" EXECUTABLE_OUTPUT_CONTENT)
string(REGEX REPLACE "This Extension is not supported by this target_signal_socket. Returning 0.
" "" EXECUTABLE_OUTPUT_CONTENT ${EXECUTABLE_OUTPUT_CONTENT})
file(WRITE executable_output.template "${EXECUTABLE_OUTPUT_CONTENT}")