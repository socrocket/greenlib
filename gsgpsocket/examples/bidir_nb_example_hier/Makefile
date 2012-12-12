# enabling SystemC2.1 automatically changes 
# includes and compiler flags in Makefile.defs
#SYSTEMC21V1 = yes

CC     = g++
OPT    = -O3
DEBUG  = -g -Wall
OTHER  = -Wno-deprecated

CFLAGS = $(OPT) $(OTHER) -DSC_INCLUDE_DYNAMIC_PROCESSES
# CFLAGS = $(DEBUG) $(OTHER)


MODULE = example_bidir
SRCS = testbench_bidir_nb.cpp
OBJS = $(SRCS:.cpp=.o)

include ../Makefile.gspm
include ../Makefile.defs

all: $(EXE)

