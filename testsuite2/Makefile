LIBS        = -lm
INC_PATH    = -I.
CC_FLAGS    = -g

all: pa31 pa32

pa31: testsuite.o pa3.o
	cc $(CC_FLAGS) $(INC_PATH) -o $@ testsuite.c $(LIBS)

pa32: testsuite.o pa3.o
	cc -DVHEAP $(CC_FLAGS) $(INC_PATH) -o $@ testsuite.c $(LIBS)

clean :
	rm -f *.o *~ $(PROGS)
