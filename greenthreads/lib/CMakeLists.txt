add_library(greenthreads SHARED
    ../src/greenthreads.cpp
    )

install(TARGETS greenthreads DESTINATION lib)
