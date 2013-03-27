include ../../Makefile.config

CC = g++
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: myprog
	@set -e; for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
	$(MAKE) -C $$i all; done

myprog: $(OBJFILES)
	mv *.o $(MYOBJDIR)

#%.o: %.cpp
#	$(CC) $(FLAGS) $(INCDIRS) -o $@ $<

