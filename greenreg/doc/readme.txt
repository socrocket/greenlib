
----------------------------------------------------------------
BUILDING DRF
----------------------------------------------------------------

The build system used to build this code is not redistributable.

The following are parameters for building the DRF source:

* GCC		3.4.4
* boost		1.33.1
* systemc	2.1.v1
* greenbus	1.1_r1_custom

* Compiler Flags:
	-DDEBUG -DSYSTEMC_2_1 -DSC_USE_STD_STRING -DLINUX -D__LINUX__
* Compiler Options:
	-m32 -g3 -O0 -Wall -c -fmessage-length=0
	
* DRF does NOT use relative paths when building, this implies that source
files use the notion #include "file.h" without specifying directory.  Thus
the directory structure needs to be added as -I/dir

The finalized release should copy all headers to a single greenreg/include directory

CHANGING THIS BEHAVIOR WILL NOT ALLOW FUTURE RELEASES TO MERGE EASILY

----------------------------------------------------------------
TEST or INCOMPLETE API's
----------------------------------------------------------------

The configuration file works fine last it was checked, however it
was disabled (commented out) in preperation for greensocs configuration
API.  The switch has not been completed.


