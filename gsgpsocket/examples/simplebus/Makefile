# enabling SystemC2.1 automatically changes 
# includes and compiler flags in Makefile.defs
#SYSTEMC21V1 = yes

ifneq ($(strip $(GSROOT)),)
  # Using GSPM, load more dependencies in Makefile.gspm
  BUILD_DEPENDS = greenrouter
endif

CC     = g++
OPT    = -O3
DEBUG  = -g -Wall
OTHER  = -Wno-deprecated

CFLAGS = $(OPT) $(OTHER) -DSC_INCLUDE_DYNAMIC_PROCESSES
# CFLAGS = $(DEBUG) $(OTHER)


MODULE = example_simplebus
SRCS = example_simplebus.cpp
OBJS = $(SRCS:.cpp=.o)

include ../Makefile.gspm
include ../Makefile.defs

all: $(EXE)

