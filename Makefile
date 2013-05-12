SRC	= malloc.h malloc.c tstalgorithms.c  \
	  tstextreme.c tstmalloc.c  tstmemory.c tstrealloc.c tstmerge.o testperformance.c testperformance.o

OBJ	= malloc.o tstalgorithms.o  \
	  tstextreme.o tstmalloc.o  tstmemory.o tstrealloc.o tstmerge.o testperformance.o

BIN	= t0 t1 t2 t3 t4 t5 t0.5 t1.5 t2.5 t3.5 t4.5 t5.5

CFLAGS	= -g -Wall -ansi -DSTRATEGY=1 -O4 -pg

XFLAGS	= -g -Wall -DSTRATEGY=1 -O4 -pg

#CC	= gcc -ansi -pedantic -Wall -g -pipe -O -pg
CC	= gcc 


all: $(BIN)
	sh RUN_TESTS

game: $(BIN)
	./t5

t0: tstmerge.o malloc.o $(X)
	$(CC) $(CFLAGS) -o  $@ tstmerge.o malloc.o $(X)

t1: tstalgorithms.o malloc.o $(X)
	$(CC) $(CFLAGS) -o  $@ tstalgorithms.o malloc.o $(X)

t2: tstextreme.o malloc.o $(X)
	$(CC) $(CFLAGS) -o $@  tstextreme.o malloc.o $(X)

t3: tstmalloc.o  malloc.o $(X)
	$(CC) $(CFLAGS) -o $@ tstmalloc.o  malloc.o $(X) 

t4: tstmemory.o malloc.o $(X)
	$(CC) $(CFLAGS) -o $@ tstmemory.o malloc.o $(X)

t5: testperformance.o malloc.o $(X)
	$(CC) $(CFLAGS) -o $@  testperformance.c malloc.o $(X)

t0.5: tstmerge.o $(X)
	$(CC) $(CFLAGS) -o  $@ tstmerge.o malloc.o $(X)

t1.5: tstalgorithms.o $(X)
	$(CC) $(CFLAGS) -o  $@ tstalgorithms.o $(X)

t2.5: tstextreme.o $(X)
	$(CC) $(CFLAGS) -o $@  tstextreme.o $(X)

t3.5: tstmalloc.o  $(X)
	$(CC) $(CFLAGS) -o $@ tstmalloc.o  $(X) 

t4.5: tstmemory.o $(X)
	$(CC) $(CFLAGS) -o $@ tstmemory.o $(X)

t5.5: testperformance.o $(X)
	$(CC) $(CFLAGS) -o $@  testperformance.c $(X)
clean:
	\rm -f $(BIN) $(OBJ) core

cleanall: clean
	\rm -f *~

