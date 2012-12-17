cmake_minimum_required(VERSION 2.8)
project (greenlib)

include(FeatureSummary)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})




# Require SystemC

find_package( SystemC )
if(SystemC_FOUND)
    include_directories(${SystemC_INCLUDE_DIRS})
endif()


set_package_properties(SystemC PROPERTIES
                       URL "http://www.accellera.org/downloads/standards/systemc"
                       DESCRIPTION "The SystemC Hardware Description Language"
                       TYPE REQUIRED
                       PURPOSE "Compile the examples")


# Require TLM
find_package( TLM )
if(TLM_FOUND)
    include_directories(${TLM_INCLUDE_DIRS})
endif()


# Require Boost

set(Boost_ADDITIONAL_VERSIONS 1.34 1.34.0 1.34.1)

find_package( Boost )
if(Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
endif()



# GreenSocs library

set (GreenSocs_INCLUDE_DIRS ../../../include)
set (GreenSocs_LIBRARY_DIRS ../../../lib)
set (GreenSocs_LIBRARIES pthread)


# License

install(FILES greencontrol/LICENSE
        DESTINATION .
)


# GreenControl

configure_file(
    greencontrol/examples/Makefile.defs.in
    Makefile.defs
    @ONLY
)

install(DIRECTORY greencontrol/include/
        DESTINATION include
        PATTERN ".svn" EXCLUDE
)

install(DIRECTORY greencontrol/examples/
        DESTINATION examples/greencontrol
        PATTERN ".svn" EXCLUDE
)

install(DIRECTORY greencontrol/docs/
        DESTINATION docs/greencontrol
        PATTERN ".svn" EXCLUDE
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Makefile.defs
        DESTINATION examples/greencontrol
)


# GreenSocket

install(DIRECTORY greensocket/include/
        DESTINATION include
        PATTERN ".svn" EXCLUDE
)

install(DIRECTORY greensocket/examples/
        DESTINATION examples/greensocket
        PATTERN ".svn" EXCLUDE
)

install(DIRECTORY greensocket/doc/     # TODO: doc or docs?
        DESTINATION docs/greensocket
        PATTERN ".svn" EXCLUDE
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Makefile.defs
        DESTINATION examples/greensocket
)


# GsgpSocket

install(DIRECTORY gsgpsocket/include/
        DESTINATION include
        PATTERN ".svn" EXCLUDE
)

install(DIRECTORY gsgpsocket/examples/
        DESTINATION examples/gsgpsocket
        PATTERN ".svn" EXCLUDE
)

# install(DIRECTORY gsgpsocket/doc/     # TODO: doc or docs?
#         DESTINATION docs/gsgpsocket
#         PATTERN ".svn" EXCLUDE
# )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Makefile.defs
        DESTINATION examples/gsgpsocket
)


# GreenRouter

install(DIRECTORY greenrouter/include/
        DESTINATION include
        PATTERN ".svn" EXCLUDE
)

install(DIRECTORY greenrouter/examples/
        DESTINATION examples/greenrouter
        PATTERN ".svn" EXCLUDE
)

install(DIRECTORY greenrouter/docs/     # TODO: doc or docs?
        DESTINATION docs/greenrouter
        PATTERN ".svn" EXCLUDE
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Makefile.defs
        DESTINATION examples/greenrouter
)


# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE  
     "${CMAKE_CURRENT_SOURCE_DIR}/greencontrol/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "0")
include (CPack)
