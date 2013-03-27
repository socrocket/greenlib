include ../Makefile.config

CC = g++
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: myprog

myprog: $(OBJFILES)
	mv *.o $(MYOBJDIR)

#%.o: %.cpp
#	$(CC) $(FLAGS) $(INCDIRS) -o $@ $<

