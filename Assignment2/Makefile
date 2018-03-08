.phony all:
all: mts #Timer

mts: mts.c
	gcc mts.c Queue.c -pthread -o mts.o

.PHONY clean:
clean:
	-rm -rf *.o *.exe
