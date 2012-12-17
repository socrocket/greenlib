# - Find TLM
# This module finds if TLM is installed and determines where the
# include files and libraries are. This code sets the following
# variables: (from kernel/sc_ver.h)
#
#  TLM_VERSION_STRING     = Version of the package found, eg. "2.2.0"
#  TLM_VERSION_MAJOR      = The major version of the package found.
#  TLM_VERSION_MINOR      = The minor version of the package found.
#  TLM_VERSION_PATCH      = The patch version of the package found.
#  TLM_VERSION_DATE       = The date of release (from TLM_VERSION)
#  TLM_VERSION            = This is set to: $major.$minor.$patch
#
# The minimum required version of TLM can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(TLM 2.2)
#
# For these components the following variables are set:
#
#  TLM_FOUND                    - TRUE if all components are found.
#  TLM_INCLUDE_DIRS             - Full paths to all include dirs.
#  TLM_LIBRARIES                - Full paths to all libraries.
#  TLM_<component>_FOUND        - TRUE if <component> is found.
#
# Example Usages:
#  FIND_PACKAGE(TLM)
#  FIND_PACKAGE(TLM 2.3)
#

#=============================================================================
# Copyright 2012 GreenSocs
#
#=============================================================================

message(STATUS "Searching for TLM")

# The HINTS option should only be used for values computed from the system.
SET(_TLM_HINTS
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\TLM\\2.2;TLMHome]/include"
  $ENV{TLM_HOME}
  )
# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_TLM_PATHS
  /usr/include/TLM
  )



set(TLM_MAJOR 2)
set(TLM_MINOR 0)
set(TLM_VERSION ${TLM_MAJOR}.${TLM_MINOR})

message(STATUS "TLM_VERSION = ${TLM_VERSION}")

set(TLM_FOUND TRUE)


FIND_PATH(TLM_INCLUDE_DIRS
  NAMES tlm.h
  HINTS ${_TLM_HINTS}
  PATHS ${_TLM_PATHS}
  PATH_SUFFIXES include/tlm
)
