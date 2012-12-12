# enabling SystemC2.1 automatically changes 
# includes and compiler flags in Makefile.defs
#SYSTEMC21V1 = yes

ifneq ($(strip $(GSROOT)),)
  # Using GSPM, load more dependencies in Makefile.gspm
  BUILD_DEPENDS = scv
endif

CC     = g++
OPT	= -O3 -funroll-loops
#OPT    = -O3 -funroll-loops -mtune=pentium4
#OPT    = -g -Wall
#OPT	= -g -Wall -fprofile-arcs -ftest-coverage
#OPT	= -g -Wall -lpthread -D_REENTRANT
#OTHER  = -Wno-deprecated

CFLAGS = $(OPT) $(OTHER)
#CFLAGS = $(DEBUG) $(OTHER)

MODULE = gclog
SRCS = UnitTests.cpp TestSetup.cpp
OBJS = $(SRCS:.cpp=.o)
#OBJS = ...

include ../../Makefile.gspm
include ../../Makefile.defs

all: $(EXE)

