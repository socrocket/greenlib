# - Find SystemC
# This module finds if SystemC is installed and determines where the
# include files and libraries are. This code sets the following
# variables: (from kernel/sc_ver.h)
#
#  SystemC_VERSION_STRING     = Version of the package found, eg. "2.2.0"
#  SystemC_VERSION_MAJOR      = The major version of the package found.
#  SystemC_VERSION_MINOR      = The minor version of the package found.
#  SystemC_VERSION_PATCH      = The patch version of the package found.
#  SystemC_VERSION_DATE       = The date of release (from SYSTEMC_VERSION)
#  SystemC_VERSION            = This is set to: $major.$minor.$patch
#
# The minimum required version of SystemC can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(SystemC 2.2)
#
# For these components the following variables are set:
#
#  SystemC_FOUND                    - TRUE if all components are found.
#  SystemC_INCLUDE_DIRS             - Full paths to all include dirs.
#  SystemC_LIBRARIES                - Full paths to all libraries.
#  SystemC_<component>_FOUND        - TRUE if <component> is found.
#
# Example Usages:
#  FIND_PACKAGE(SystemC)
#  FIND_PACKAGE(SystemC 2.3)
#

#=============================================================================
# Copyright 2012 GreenSocs
#
#=============================================================================

message(STATUS "Searching for SystemC")

# The HINTS option should only be used for values computed from the system.
SET(_SYSTEMC_HINTS
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SystemC\\2.2;SystemcHome]/include"
  $ENV{SYSTEMC_PREFIX}/include
  $ENV{SYSTEMC_PREFIX}/lib
  $ENV{SYSTEMC_PREFIX}/lib-linux
  $ENV{SYSTEMC_PREFIX}/lib-linux64
  $ENV{SYSTEMC_PREFIX}/lib-macos
  )
# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_SYSTEMC_PATHS
  /usr/include/systemc
  /usr/lib
  /usr/lib-linux
  /usr/lib-linux64
  /usr/lib-macos
  /usr/local/lib
  /usr/local/lib-linux
  /usr/local/lib-linux64
  /usr/local/lib-macos
  )
FIND_FILE(_SYSTEMC_VERSION_FILE
  NAMES sc_ver.h
  HINTS ${_SYSTEMC_HINTS}
  PATHS ${_SYSTEMC_PATHS}
  PATH_SUFFIXES sysc/kernel
)




message(STATUS "_SYSTEMC_VERSION_FILE   in   ${_SYSTEMC_VERSION_FILE}")


# IF(_SYSTEMC_VERSION_FILE)
#     EXECUTE_PROCESS(COMMAND "echo sed -ne 's,^#define\\s*SYSTEMC_VERSION\\s*,,p' ${_SYSTEMC_VERSION_FILE}"
#       RESULT_VARIABLE res
#       OUTPUT_VARIABLE var
#       ERROR_VARIABLE var # sun-java output to stderr
#       OUTPUT_STRIP_TRAILING_WHITESPACE
#       ERROR_STRIP_TRAILING_WHITESPACE)
#     if(res)
#         MESSAGE(STATUS "found version ${res},${var}.")
#     endif()
# endif()


set(SystemC_MAJOR 2)
set(SystemC_MINOR 2)
set(SystemC_VERSION ${SystemC_MAJOR}.${SystemC_MINOR})

message(STATUS "SystemC_VERSION = ${SystemC_VERSION}")

FIND_PATH(SystemC_INCLUDE_DIRS
  NAMES systemc.h
  HINTS ${_SYSTEMC_HINTS}
  PATHS ${_SYSTEMC_PATHS}
)

FIND_PATH(SystemC_LIBRARY_DIRS
  NAMES libsystemc.a
  HINTS ${_SYSTEMC_HINTS}
  PATHS ${_SYSTEMC_PATHS}
)

set(SystemC_LIBRARIES ${SystemC_LIBRARY_DIRS}/libsystemc.a)

message(STATUS "SystemC library = ${SystemC_LIBRARIES}")

set(SystemC_FOUND TRUE)
