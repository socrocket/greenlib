include Makefile.config


SUBDIRS = framework sysc_ext greenreg_socket internal utils
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: myprog
	@set -e; for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
	$(MAKE) -C $$i all; done
	ar -r "libgreenreg.a" $(MYOBJDIR)/*.o
	rm -f  $(MYOBJDIR)/*.o

myprog: $(OBJFILES)
	mv *.o $(MYOBJDIR)

%.o: %.cpp
	$(CC) $(FLAGS) $(INCDIRS) -o $@ $<

clean:
	rm -f  $(MYOBJDIR)/*.o libgreenreg.a

