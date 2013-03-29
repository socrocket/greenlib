add_executable(greenreg_simple simple/sc_main.cpp)
target_link_libraries(greenreg_simple ${SystemC_LIBRARIES} greenreg pthread)
add_executable(greenreg_container reg_container/sc_main.cpp)
target_link_libraries(greenreg_container ${SystemC_LIBRARIES} greenreg pthread)
