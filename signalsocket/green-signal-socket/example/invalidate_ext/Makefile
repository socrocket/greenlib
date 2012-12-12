# Directory where this Makefile is located
TOP := $(dir $(lastword $(MAKEFILE_LIST)))

GREENSIGNAL_DIR := $(TOP)/../include

# Try to find dependencies automatically
-include $(TOP)/../../../Makefile.greensocs


## Your compiler
CXX    ?= g++
OPT    ?= -O3
DEBUG  ?= -g -Wall
OTHER  ?= -Wno-deprecated
CFLAGS ?= $(DEBUG) $(OTHER) 

MODULE = example

## set some directories 
TLM2_DIR ?= /local/workspace/greensocs/gspm/OSCI/tlm/include/tlm
GREENSOCKET_DIR ?= /local/workspace/greensocs/packages
GSGPSOCKET_DIR ?= /local/workspace/greensocs/packages
GS_CONTROL_DIR ?= /local/workspace/greensocs/packages
GREENREG_DIR ?= /local/workspace/greensocs/packages
SYSTEMC_DIR?=/local/workspace/greensocs/gspm/OSCI/systemc/2.2/gcc4.0
BOOST ?= /cad/tools/boost

LDFLAGS =

INCDIR = -I$(GS_CONTROL_DIR) -I$(BOOST) -I$(GREENSOCKET_DIR) -I$(GSGPSOCKET_DIR) -I$(GREENSIGNAL_DIR)\
         -I$(TLM2_DIR) -I. -I.. -I$(SYSTEMC_DIR)/include 
SYSTEMC_DIR      = /home/puneet/gsocs/gspm/OSCI/systemc/2.2/gcc4.0
TLM2_DIR          = /home/puneet/gsocs/gspm/OSCI/tlm/include/tlm
GREENSRC          = /home/puneet/gsocs/latest-svn-packages
GREENREG_DIR      = $(GREENSRC)/GreenReg/greenreg
GREENSIGNALBUS    = $(GREENSRC)/SignalSocket/signalsocket/
GREENSIGNALSOCKET = $(GREENSRC)/SignalSocket/signalsocket/green-signal-socket/include
GSGPSOCKET_DIR    = $(GREENSRC)/GSGPSocket
GREENSOCKET_DIR   = $(GREENSRC)/GreenSocket
GREENCONTROL_DIR  = $(GREENSRC)/GreenControl

LDFLAGS =

INCDIR = -I$(GREENCONTROL_DIR)  -I$(GREENSOCKET_DIR) -I$(GSGPSOCKET_DIR) -I$(GREENSIGNALBUS) -I$(GREENSIGNALSOCKET)\
         -I$(TLM2_DIR) -I. -I.. -I$(SYSTEMC_DIR)/include 
LIBDIR = -L$(SYSTEMC_DIR)/lib-linux

LIBS   =  -lsystemc

SRCS = example.cpp

OBJS = $(SRCS:.cpp=.o)

EXE    = $(MODULE).x



$(EXE): $(OBJS)  
	$(CXX) $(CFLAGS)  $(LDFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(OBJS) $(LIBS) 2>&1 | c++filt
#	strip $(EXE)

.cpp.o:
	$(CXX) $(CFLAGS)  $(INCDIR) -c $<

clean::
	rm -f $(OBJS) *~ $(EXE) core
