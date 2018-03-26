add_library(greenthreads STATIC
    ../src/greenthreads.cpp
    )

target_link_libraries(greenthreads ${SystemC_LIBRARIES})

install(TARGETS greenthreads DESTINATION lib)
