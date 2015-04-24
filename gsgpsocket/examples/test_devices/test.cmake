# Read executed output
file(READ "executable_output.txt" EXECUTABLE_OUTPUT_CONTENT)

# Replace "In file: *" lines
string(REGEX REPLACE "(In file: ([^\n]*):[0-9]+)
" "" EXECUTABLE_OUTPUT_CONTENT ${EXECUTABLE_OUTPUT_CONTENT})

file(WRITE executable_output.template "${EXECUTABLE_OUTPUT_CONTENT}")