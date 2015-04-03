# Read executed output
file(READ "executable_output.txt" EXECUTABLE_OUTPUT_CONTENT)

# Find and replace S0 ID
string(REGEX REPLACE ".*S0 using tag ([0-9]+) for my socket.*" "\\1"
       ID0 ${EXECUTABLE_OUTPUT_CONTENT})
string(REGEX REPLACE ${ID0} "{ID0}" EXECUTABLE_OUTPUT_CONTENT
       ${EXECUTABLE_OUTPUT_CONTENT})

# Find and replace S1 ID
string(REGEX REPLACE ".*S1 using tag ([0-9]+) for my socket.*" "\\1"
       ID1 ${EXECUTABLE_OUTPUT_CONTENT})
string(REGEX REPLACE ${ID1} "{ID1}" EXECUTABLE_OUTPUT_CONTENT
      ${EXECUTABLE_OUTPUT_CONTENT})

string(REGEX REPLACE
     "cacheable used as rejected.\n    priority used as rejected.|priority used as rejected.\n    cacheable used as rejected."
     "{CACHEABLE_PRIORITY_REJECTED}" EXECUTABLE_OUTPUT_CONTENT ${EXECUTABLE_OUTPUT_CONTENT})

string(REGEX REPLACE
       "priority used as optional.\n    cacheable used as optional.|cacheable used as optional.\n    priority used as optional."
       "{PRIORITY_CACHEABLE_OPTIONAL}" EXECUTABLE_OUTPUT_CONTENT ${EXECUTABLE_OUTPUT_CONTENT})

string(REGEX REPLACE
      "priority used as rejected.\n    cacheable used as mandatory.|cacheable used as mandatory.\n    priority used as rejected."
      "{PRIORITY_REJECTED_CACHEABLE_MANDATORY}" EXECUTABLE_OUTPUT_CONTENT ${EXECUTABLE_OUTPUT_CONTENT})

string(REGEX REPLACE
       "cacheable used as rejected.\n    priority used as mandatory.|priority used as mandatory.\n    cacheable used as rejected."
       "{PRIORITY_MANDATORY_CACHEABLE_REJECTED}" EXECUTABLE_OUTPUT_CONTENT ${EXECUTABLE_OUTPUT_CONTENT})

file(WRITE executable_output.template "${EXECUTABLE_OUTPUT_CONTENT}")
