# enabling SystemC2.1 automatically changes 
# includes and compiler flags in Makefile.defs
#SYSTEMC21V1 = yes

SYSTEMC_HOME    = /home/manish/Packages/systemc-2.2.0
TLM_HOME        = /home/manish/Packages/TLM-2008-06-09
GSGPSOCKET_DIR   = /home/manish/Projects
ARCH_TARGET     = linux

SYSTEMC_INC_DIR = $(SYSTEMC_HOME)/include
SYSTEMC_LIB_DIR = $(SYSTEMC_HOME)/lib-$(ARCH_TARGET)
TLM_INC_DIR     = $(TLM_HOME)/include/tlm

BOOST_HOME_DIR = /home/manish/install/gspm/Boost/boost
BOOST_INC_DIR = $(BOOST_HOME_DIR)/include/boost-1_34_1
BOOST_FS_LIB_DIR = $(BOOST_HOME_DIR)/lib
BOOST_FS_LIB_NAME= boost_filesystem-gcc40-1_34_1

GXX             = g++
GCC             = gcc
CC              = gcc
LD              = g++

SRCDIR  =   .
SRCS = example_simple.cpp
OBJS = $(SRCS:.cpp=.o)
OBJ_NAME = example_simple

FLAGS    = -g -DSC_USE_STD_STRING -DUSE_GPSOCKET
LDFLAGS  = -lm -L$(SYSTEMC_LIB_DIR) -L$(BOOST_FS_LIB_DIR) -lsystemc -lboost_filesystem-gcc40-1_34_1

INCDIRS  =  -I $(SYSTEMC_INC_DIR)  \
	    -I $(BOOST_INC_DIR)  \
            -I $(TLM_INC_DIR)   \
            -I $(GSGPSOCKET_DIR)

CFLAGS  =   $(FLAGS) $(INCDIRS) -DSC_INCLUDE_DYNAMIC_PROCESSES
	    
.PHONY: clean all check

all: announce $(OBJ_NAME).exe

announce:
	@if  ( test ! -d $(FLAG_BATCH) ); \
	 	then echo; echo "$(OBJ_NAME):"; echo; \
	 fi

check: announce $(OBJ_NAME).exe
	@$(VALGRIND) ./$(OBJ_NAME).exe > run.log
	@diff run.log ../results/expected.log > diff.log 2>&1;\
	if [ -s diff.log ]; then echo "***ERROR:"; cat diff.log; else echo "OK"; fi

run: announce $(OBJ_NAME).exe
	@./$(OBJ_NAME).exe

$(OBJ_NAME).exe: $(OBJS)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_NAME).o: $(SRCDIR)/$(OBJ_NAME).cpp
	$(GXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *~ *.o *.log $(OBJ_NAME).exe fbe_buffer fbe_registers result.gif

