include ../Makefile.config

FLAGS     =  -g -DSC_USE_STD_STRING
#FLAGS     =  -g -Werror -DSC_USE_STD_STRING
#LDFLAGS   =  -lm -L$(SYSTEMC_LIB_DIR) -L$(BOOST_FS_LIB_DIR) -L$(GREENREG_DIR) -L$(LUA_LIB_DIR) -l$(BOOST_FS_LIB_NAME) -lgreenreg -lsystemc -llua
LDFLAGS   =  -lm -L$(SYSTEMC_LIB_DIR) -lsystemc

INCDIRS   =  -I../include   \
             -I$(SYSTEMC_INC_DIR)  \
             -I$(TLM_INC_DIR) \
             -I$(GSGPSOCKET_DIR) \
             -I$(GREENSOCKET_DIR) \
             -I$(GREENCONTROL_DIR) \
             -I$(GREENROUTER_DIR) \
             -I$(GREENSIGNALBUS) \
             -I$(GREENSIGNALSOCKET)

CFLAGS    = $(FLAGS) $(INCDIRS) -DSC_INCLUDE_DYNAMIC_PROCESSES
SRCDIR    = ../src
OBJ_NAME  = example
OBJS      = $(OBJ_NAME).o

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

#PICCore.o: $(SRCDIR)/PICCore.cpp
#	$(GXX) $(CFLAGS) -c $< -o $@

$(OBJ_NAME).o: $(OBJ_NAME).cpp
	$(GXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *~ *.o *.log $(OBJ_NAME).exe
