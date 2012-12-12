## Your compiler
CC = g++
OPT    = -O3
DEBUG  = -g -Wall
OTHER  = -Wno-deprecated
CFLAGS = $(OPT) $(OTHER) 

MODULE = multi_example

## set some directories 
TLM2_DIR = /local/workspace/greensocs/gspm/OSCI/tlm/include/tlm
GREENSOCKET_DIR = /local/workspace/greensocs/packages
GSGPSOCKET_DIR = /local/workspace/greensocs/packages
GS_CONTROL_DIR = /local/workspace/greensocs/packages
GREENREG_DIR = /local/workspace/greensocs/packages
SYSTEMC_DIR=/local/systemc-2.2.0/systemc-2.2.0
GREENSIGNAL_DIR=/local/workspace/Green_Signal/include

LDFLAGS =

INCDIR = -I$(GS_CONTROL_DIR)  -I$(GREENSOCKET_DIR) -I$(GSGPSOCKET_DIR) -I$(GREENSIGNAL_DIR)\
         -I$(TLM2_DIR) -I. -I.. -I$(SYSTEMC_DIR)/include 
LIBDIR = -L$(SYSTEMC_DIR)/lib-linux

LIBS   =  -lsystemc

SRCS = multi_example.cpp

OBJS = $(SRCS:.cpp=.o)

EXE    = $(MODULE).x



$(EXE): $(OBJS)  
	$(CC) $(CFLAGS)  $(LDFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(OBJS) $(LIBS) 2>&1 | c++filt
#	strip $(EXE)

.cpp.o:
	$(CC) $(CFLAGS)  $(INCDIR) -c $<

clean::
	rm -f $(OBJS) *~ $(EXE) core