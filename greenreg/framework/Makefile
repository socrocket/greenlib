include ../Makefile.config


SUBDIRS = core devices registers
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: myprog
	@set -e; for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
	$(MAKE) -C $$i all; done

myprog: $(OBJFILES)

%.o: %.cpp
	$(CC) $(FLAGS) $(INCDIRS) -o $@ $<

