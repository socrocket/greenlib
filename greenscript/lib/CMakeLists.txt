add_library(greenscript STATIC ../src/greenscript.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_GREENCONTROL -Wno-unused-value")

#
# Create _gsp_sc and _gsp_sc.py for GreenScript.
#

### SWIG (gsp_scPYTHON_wrap) ###
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/../gsp_scPYTHON_wrap.cxx
    COMMAND ${SWIG_EXECUTABLE} -python -c++ -DUSE_GREENCONTROL
            -o ${CMAKE_CURRENT_BINARY_DIR}/../gsp_scPYTHON_wrap.cxx
            -outdir ${CMAKE_CURRENT_BINARY_DIR}
            ../gsp_sc.i
    DEPENDS ../gsp_sc.i)
add_custom_target(gsp_scPYTHON_wrap
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../gsp_scPYTHON_wrap.cxx)

### gsp_sc ###
add_library(_gsp_sc SHARED ../src/gsp_sc.cpp
                           ${CMAKE_CURRENT_BINARY_DIR}/../gsp_scPYTHON_wrap.cxx)
set_target_properties(_gsp_sc PROPERTIES PREFIX "")
add_dependencies(_gsp_sc gsp_scPYTHON_wrap)
target_link_libraries(_gsp_sc ${LUA_LIBRARIES}
                              ${PYTHON_LIBRARIES}
                              ${SystemC_LIBRARIES})

#
# INSTALL
#
install(TARGETS greenscript DESTINATION lib)
install(FILES ../gs.py
        DESTINATION "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages")

#
# This is generated files, there are in CMAKE_CURRENT_BINARY_DIR.
#
install(TARGETS _gsp_sc
        DESTINATION "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gsp_sc.py
        DESTINATION "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages")
